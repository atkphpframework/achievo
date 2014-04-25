<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage setup
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * Class for displaying the license file
 *
 * @package achievo
 * @subpackage setup
 */
class license extends atkNode
{

    /**
     * Constructor
     *
     * @return void
     */
    function license()
    {
        $this->atkNode("license", NF_NO_SECURITY);
    }

    /**
     * Create custom view page with the license
     *
     * @param object $handler Handler
     */
    function action_view(&$handler)
    {
        $ui = &$this->getUi();
        if (is_object($ui)) {
            $theme = &atkTheme::getInstance();
            $page = &$this->getPage();
            $page->register_style($theme->stylePath("style.css"));
            $page->head(atktext("cicence", "setup"));
            $page->body();


            $box = $ui->renderBox(array("title" => $this->text($this->m_type) . " - " . $this->text($this->m_view),
                "content" => $this->getLicenseText()));
            $actionpage = $this->renderActionPage("view", array($box));
            $page->addContent($actionpage);
        } else {
            atkerror("ui object failure");
        }
    }

    /**
     * Get license file from the doc dir
     *
     * @return string
     */
    function getLicenseText()
    {
        $license = file(atkconfig("atkroot") . "doc/LICENSE");
        $content = "";
        for ($i = 0; $i < count($license); $i++) {
            $content.='<br>' . str_replace("", "", $license[$i]);
        }
        return $content;
    }

}

?>