<?php
/**
 * Moonphases class file
 *
 * @package achievo
 * @subpackage modules
 *
 * @author Sandy <sandy@ibuildings.nl>
 *
 * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
 *
 * @version $Revision$
 * $Id$
 */

class moonphases
{
	/**
	 * Return the moon phase for a given timestamp
	 * A moon phase can be:
	 * - ne (Full moon)
	 * - ny (No moon)
	 * - ne2 (Half moon)
	 * - ny2 (Half moon)
	 * or nothing
	 *
	 * @param int $timestamp
	 * @return string Moonphase 
	 */
	function getMoonPhase($timestamp)
	{
	  static $moon_phases;
	  if (count($moon_phases)==0) $moon_phases=moonphases::calculateMoonPhases(date("Y",$timestamp),date("d",$timestamp));
	  $day=date("Ymd",$timestamp);
	  return $moon_phases[$day];
	}
	
	/**
	 * Returns an array with all the phases of the moon for a whole year
	 * 
	 * Code taken from http://www.zend.com/codex.php?id=830&single=1
   * Added new param to return the moon phases for only 1 month
   * and changed the return array in Ymd format.
	 * Converted from Basic by Roger W. Sinnot, Sky & Telescope, March 1985.
	 * Converted from javascript by Are Pedersen 2002
	 * Javascript found at http://www.stellafane.com/moon_phase/moon_phase.htm
	 * 
	 * @param int $Y Year
	 * @return array Array with the moonphases of the given year
	 */
	function calculateMoonPhases( $Y, $month=1) 
	{
	  $R1 = 3.14159265 / 180;
	  $U = false;
	  $s = ""; // Formatted Output String
	  $K0 = intval(($Y-1900)*12.3685)+($month-1);
	  $T = ($Y-1899.5) / 100;
	  $T2 = $T*$T; $T3 = $T*$T*$T;
	  $J0 = 2415020 + 29*$K0;
	  $F0 = 0.0001178*$T2 - 0.000000155*$T3;
	  $F0 += (0.75933 + 0.53058868*$K0);
	  $F0 -= (0.000837*$T + 0.000335*$T2);
	  //X In the Line Below, F is not yet initialized, and J is not used before it set in the FOR loop.
	  //X J += intval(F); F -= INT(F);
	  //X Ken Slater, 2002-Feb-19 on advice of Pete Moore of Houston, TX
	  $M0 = $K0*0.08084821133;
	  $M0 = 360*($M0 - intval($M0)) + 359.2242;
	  $M0 -= 0.0000333*T2;
	  $M0 -= 0.00000347*T3;
	  $M1 = $K0*0.07171366128;
	  $M1 = 360*($M1 - intval($M1)) + 306.0253;
	  $M1 += 0.0107306*$T2;
	  $M1 += 0.00001236*$T3;
	  $B1 = $K0*0.08519585128;
	  $B1 = 360*($B1 - intval($B1)) + 21.2964;
	  $B1 -= 0.0016528*$T2;
	  $B1 -= 0.00000239*$T3;
	  for ( $K9=0; $K9 <= 28; $K9=$K9+0.5 ) 
	  {
	    $J = $J0 + 14*$K9; $F = $F0 + 0.765294*$K9;
	    $K = $K9/2;
	    $M5 = ($M0 + $K*29.10535608)*$R1;
	    $M6 = ($M1 + $K*385.81691806)*$R1;
	    $B6 = ($B1 + $K*390.67050646)*$R1;
	    $F -= 0.4068*sin($M6);
	    $F += (0.1734 - 0.000393*$T)*sin($M5);
	    $F += 0.0161*sin(2*$M6);
	    $F += 0.0104*sin(2*$B6);
	    $F -= 0.0074*sin($M5 - $M6);
	    $F -= 0.0051*sin($M5 + $M6);
	    $F += 0.0021*sin(2*$M5);
	    $F += 0.0010*sin(2*$B6-$M6);
	    $F += 0.5 / 1440; //Adds 1/2 minute for proper rounding to minutes per Sky & Tel article
	    $J += intval($F); $F -= intval($F);
	    //Convert from JD to Calendar Date
	    $julian=$J+round($F);
	    if (function_exists('jdtogregorian'))
	    {
        $s = date ('Ymd',strtotime(jdtogregorian($julian)));
      }
      else 
      {
        $s = jd_to_greg($julian);
      } 
	    //half K
	    if (($K9-floor($K9))>0)
	    {
	      if (!$U)
	      {
	        //New half
	        $phases[$s]="ny2";
	      } 
	      else
	      {
	        //Full half
	        $phases[$s]="ne2";
	      }
	    }
	    else
	    {
	      //full K
	      if ( !$U )
	      {
	        // New
	        $phases[$s]="ny";
	      }
	      else
	      {
	        // Full
	        $phases[$s]="ne";
        }
        $U = !$U;
	    }
	  } // Next
	  return $phases;
	} 
	
	/**
	 * Convert julian date to gregorian
	 * function taken from http://us3.php.net/manual/en/function.jdtogregorian.php
	 * used if calendar functions are not compiled in php
	 * 
	 * @param int $julian
	 * @return string Date string (YYYYMMDD)
	 */
  function jd_to_greg($julian) 
  {
    $julian = $julian - 1721119;
    $calc1 = 4 * $julian - 1;
    $year = floor($calc1 / 146097);
    $julian = floor($calc1 - 146097 * $year);
    $day = floor($julian / 4);
    $calc2 = 4 * $day + 3;
    $julian = floor($calc2 / 1461);
    $day = $calc2 - 1461 * $julian;
    $day = floor(($day + 4) / 4);
    $calc3 = 5 * $day - 3;
    $month = floor($calc3 / 153);
    $day = $calc3 - 153 * $month;
    $day = floor(($day + 5) / 5);
    $year = 100 * $year + $julian; 

    if ($month < 10) {
        $month = $month + 3;
    }
    else {
        $month = $month - 9;
        $year = $year + 1;
    }
    return sprintf ( "%04d%02d%02d", $year, $month, $day);
  }

} 
?>