<?php

/**
small "Driekoningen" on 6.1
small "Vastenavond" on easter minus 47 days
small "Aswoensdag" on easter minus 46 days
small "Palmzondag" on easter minus 7 days
small "Witte Donderdag" on easter minus 3 days
small "Goede Vrijdag" on easter minus 2 days
small "Dodenherdenking" on 4.5
small "Bevrijdingsdag" on 5.5
small "Dierendag" on 4.10
small "Allerheiligen" on 1.11
small "Allerzielen" on 2.11
small "Oudjaar" on 31.12
small "Moederdag" on every second sunday in may
small "Vaderdag" on every third sunday in june
blue "Zomertijd" on last sunday in march
blue "Wintertijd" on last sunday in october
*/

$holidays_nl = array("Nieuwjaar"=>array("type"=>"weekend",
                                        "month"=>1,
                                        "day"=>1),
                     "Goede vrijdag"=>array("special"=>"easter",
                                            "offset"=>"-2"),
                     "Pasen"=>array("type"=>"weekend",
                                    "special"=>"easter",
                                    "length"=>2),
                     "Hemelvaart"=>array("type"=>"weekend",
                                         "special"=>"easter",
                                         "offset"=>39),
                     "Pinksteren"=>array("type"=>"weekend",
                                         "special"=>"easter",
                                         "offset"=>49,
                                         "length"=>2),
                     "Koninginnedag"=>array("type"=>"weekend",
                                            "month"=>4,
                                            "day"=>30),
                     "Bevrijdingsdag"=>array("month"=>5,
                                             "day"=>5),
                     "Kerstmis"=>array("month"=>12,
                                       "day"=>25,
                                       "length"=>2)
                     );
?>
