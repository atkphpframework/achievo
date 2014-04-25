<?php

/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage scheduler
 *
 * @copyright (c)2008 Sandy Pleyte
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
class xmlfunctions
{

    function xmlfunctions()
    {
        
    }

    function exportXML($holidays)
    {
        if (count($holidays) == 0)
            return "";
        return "<xml>
              <test>dit is een test</test>
            </xml>";
    }

    function importXML()
    {
        
    }

}

?>