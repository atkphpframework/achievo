#
# Description   : Transfers all registered hours on a project in a specified 
#                 time frame from one phase to another.
#
# Usage         : Run with 'perl phase_convert.pl -h' for help.
# 
# Contributed by: Maciej Maczynski <macmac@xdsnet.pl>
# Version       : $Revision$
#
# $Id$
#
use DBI;
use Getopt::Long;
use Time::Local;

$startdate = "";
$stopdate = "";
$userid = "";
$project = "";
$oldphase = "";
$newphase = "";
$oldphaseid = 0;
$newphaseid = 0;
$database = "achievo_0_8";
$dbuser = "achievo";
$dbpassword = "";
$help = 0;
$write  = 0;

$tracesql = 0;


%optctl = ( "database" => \$database,
            "dbuser"   => \$dbuser,
            "dbpassword" => \$dbpassword,
            "user"  => \$userid,
	    "start" => \$startdate,
	    "stop"  => \$stopdate,
	    "project" => \$project,
	    "oldphase" => \$oldphase,
	    "newphase" => \$newphase,
	    "debug"   => \$tracesql,
            "write"   => \$write,
            "help"    => \$help
	    );

GetOptions( \%optctl,
            "database=s",
            "dbuser=s",
            "dbpassword=s",
	    "user=s",
	    "start=s",
	    "stop=s",
	    "project=s",
	    "oldphase=s",
	    "newphase=s",
	    "debug!",
            "write!",
            "help!"
	    );

sub PrintUsage {
  print qq{
Project phase conversion utility.
Usage:
   perl phase_convert.pl <options>

Options:
   --start      : period start in YYYY-MM-DD format. Mandatory.
   --stop       : period end in YYYY-MM-DD format. If not specifed - same as --start.
   --database   : name of achievo database. Default: $database.
   --dbuser     : name database user. Default: $dbuser.
   --dbpassword : database access password. Default: '$dbpassword'.
   --user       : achievo user id. If not specified - all users.
   --project    : project name. Mandatory.
   --oldphase   : old phase name. Mandatory.
   --newphase   : new phase name. Mandatory.
   --write      : write changes to database.
   --debug      : SQL trace (partial).

   Short options are allowed, e.g. --user/-u).
}
};

%optctl = ( "user"  => \$userid,
	
	    "project" => \$project,
	    "oldphase" => \$oldphase,
	    "newphase" => \$newphase,
	    "debug"   => \$tracesql,
            "write"   => \$write,
	    );

sub Execute {
  my ($stat) = @_;

  if ( $tracesql ) { print "$stat\n"; };
  $queryhandle = $dbh -> prepare( $stat )
  || die "Can't prepare : $DBI::errstr";
  my $rc = $queryhandle -> execute
  || die "Can't execute : $DBI::errstr";
}

sub Fetch {
  my @result;

  @result = $queryhandle -> fetchrow_array();
  if ( ! @result ) 
  {
    $queryhandle -> finish;
  }
  return @result;
}

sub GetPhaseId {
  my ($phase_name, $project_name) = @_;
  Execute( "SELECT ph.id from phase ph, project pr  WHERE ph.name='$phase_name' ".
           "AND pr.name='$project_name' AND ph.projectid=pr.id");    
  my ($id) = Fetch();
  return $id;
}


sub AdjustDates {
  if ( $startdate eq "" )
  {
    # set "now" as default start date
    Execute( "select DATE_FORMAT( now(), \"%Y-%m-%d\")" );
    ($stopdate) = Fetch();	
    $startdate = $stopdate;
  }

  if ( $stopdate eq "" )
  {
    # if stopdate not specified, set it equl to startdate
    $stopdate = $startdate;
  }
}

# $_[0] - user name
# output - 1 : user exists
#          0 : no such user
sub CheckUser {
  my $exists;
  my $email;
  my $username;

  my $sth = $dbh -> prepare( "select email, name from employee where userid = '$_[0]'" )
  || die "Can't prepare : $DBI::errstr";
  my $rc = $sth -> execute
  || die "Can't execute : $DBI::errstr";

  if ( ($email, $username ) = $sth -> fetchrow_array() )
  {
    $exists = 1;
  }
  else
  {
    $exists = 0;
  }
  $sth->finish;    

  $exists, $email, $username;
}


# $_[0] - startdate
# $_[1] - stopdate
sub SetForAllUsers
{
  my $id;
  my $name;

  my $sth = $dbh -> prepare( "select userid,name from employee" )
  || die "Can't prepare : $DBI::errstr";
  my $rc = $sth -> execute
  || die "Can't execute : $DBI::errstr";

  while ( ($id, $name) = $sth -> fetchrow_array )
  {
    if ( $id ne "administrator" )
    {
      SetForUser( $id, $_[0], $_[1] );
    }
  }
  $sth->finish;        
}


# $_[0] - user id
# $_[1] - start date
# $_[2] - stop date
sub SetForUser
{
  my ($userid, $start, $stop) = @_;
  my $user_exists;
  my $useremail;
  my $username;

  # Check if user exists
  ( $user_exists, $useremail, $username ) = CheckUser( $userid );
  if ( $user_exists ) {
    print "Updating $username,'$userid':\n";
    #   for given user,
    #       for given project,
    #                 for given current phase id
    #                    for all entries in hours table between start and stop date
    #                       set phaseid to id of new phase
    if ( $write ) {
      Execute( "UPDATE hours SET phaseid=$newphaseid WHERE userid='$userid' AND phaseid=$oldphaseid AND ".
               "activitydate >= '$startdate' AND activitydate <= '$stopdate'" );
    }
    else {
      Execute( "select id, activitydate from hours WHERE userid='$userid' AND phaseid=$oldphaseid AND ".
               "activitydate >= '$startdate' AND activitydate <= '$stopdate' ORDER BY activitydate" );
      my $didit = 0;
      while ( ($id, $date) = Fetch() )
      {
        print "$id, $date\n";
        $didit = 1;
      }
      if ( !$didit ) { print "no activities\n"; }
    }
  }
  else
  {
    print "User $_[0] does not exist.\n";
    print "The following users are defined:\n";
    ListUsers();
  }
}


if ( !$help ) 
{
  if ( $project eq "" ) {die("missing project name");}
  if ( $oldphase eq "" ) {die("missing current phase name");}
  if ( $newphase eq "" ) {die("missing new phase name");}

  $dbh = DBI -> connect( "dbi:mysql:$database", $dbuser, $dbpassword );
  if ( $dbh )
  {
    AdjustDates();

    $oldphaseid = GetPhaseId( $oldphase, $project );
    $newphaseid = GetPhaseId( $newphase, $project );

    print "Project: $project\n";
    print "Changing phase '$oldphase'($oldphaseid) to '$newphase'($newphaseid)\n";
    print "Period:  $startdate - $stopdate\n\n";
    my $proceed = 1;

    if ( $write ) {
      print "You are going to make changes in your achievo database\n";
      print "Did you make a backup of your database? Are you sure?\n";
      print "Really? [y/n] ";
      my $c = getc;
      if ($c ne 'y' ) { $proceed = 0; }
    } else {
      print "Running int info-only mode. No changes will be written to database.\n";
      print "Run the script with --write (-w) options to do the update.\n\n";
    }

    if ( ! $oldphaseid ) { die("Phase '$oldphase' does not exist for project '$project'"); }
    if ( ! $newphaseid ) { die("Phase '$newphase' does not exist for project '$project'"); }

    if ( $proceed ) {
      if ( $userid eq "" ) {	
        SetForAllUsers( $startdate, $stopdate);
      }
      else {
        SetForUser( $userid, $startdate, $stopdate );
      }
    }
  }    
  else
  {
    print $DBI::errstr; 
  }
} else { PrintUsage(); }






