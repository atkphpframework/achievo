<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage employee
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
useattrib("atkdummyattribute");
useattrib("atknumberattribute");
useattrib("atkprofileattribute");

/**
 * Class for managing profiles
 * @package achievo
 * @subpackage employee
 */
class profile extends atkNode
{

    /**
     * Constructor
     */
    function profile()
    {
        $this->atkNode("profile", NF_EDITAFTERADD | NF_NO_VIEW); // view disabled as long as profileattribute does not support display

        $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
        $this->add(new atkAttribute("name", AF_OBLIGATORY | AF_UNIQUE | AF_SEARCHABLE, 50));
        $this->add(new atkDummyAttribute("profile_explanation", atktext("profile_explanation"), AF_HIDE_LIST | AF_HIDE_ADD));
        $this->add(new atkProfileAttribute("accessrights", AF_HIDE_LIST | AF_HIDE_ADD | AF_BLANKLABEL));

        $this->setTable("profile", "profile");
        $this->setOrder("name");
    }

    /**
     * Descriptor template
     *
     * @return string
     */
    function descriptor_def()
    {
        return "[name]";
    }

}

?>