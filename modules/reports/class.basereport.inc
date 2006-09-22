<?php
  /**
   * basereport node class file
   *
   * Basereport on it self is not a working report, but a basis to
   * create specific reports easily. It provides some common interface
   * functions to draw the filter attributes, retrieve the filter record
   * etc.
   *
   * @package achievo
   * @subpackage modules
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision$
   * $Id$
   */

  define("RT_STANDARD", 0);
  define("RT_PRINTABLE", 1);
  define("RT_EXPORT", 2);

  /**
   * The node class for basereport.
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo
   * @subpackage modules
   */
  class basereport extends atkNode
  {
    /**
     * basereport constructor
     */
    function basereport($name, $flags=0)
    {
      $this->atkNode($name, $flags);

      // Add the output types attribute
      //$outputtypes = $this->getOutputTypes();
      //$this->add(new atkListAttribute("report_output_type", array_values($outputtypes), array_keys($outputtypes), AF_OBLIGATORY), null, 99999);
    }

    function getOutputTypes()
    {
      $reporttypes = array(
        RT_STANDARD => "report_output_standard",
        RT_PRINTABLE => "report_output_printable",
        RT_EXPORT => "report_output_export",
      );
      return $reporttypes;
    }

    function isGo()
    {
      static $s_go = null;
      if ($s_go == null)
      {
        global $g_sessionManager;
        $s_go = ($g_sessionManager->pageVar("go") == 1);
      }
      return $s_go;
    }

    function &getFilterRecord()
    {
      // Cache the record so we don't have to fetch it over and over
      static $s_record = NULL;

      // If the record isn't cached yet, load it
      if ($s_record==NULL)
      {
        if ($this->isGo())
        {
          global $g_sessionManager;

          // Get a list of all attribute keys
          $attrkeys = array_keys($this->m_attribList);

          // Load all values from request
          $r_record = array();
          for($i=0,$_i=count($attrkeys); $i<$_i; $i++)
          {
            $r_record[$attrkeys[$i]] = $g_sessionManager->pageVar($attrkeys[$i]);
          }

          // Load all values from request
          $s_record = array();
          for($i=0,$_i=count($attrkeys); $i<$_i; $i++)
          {
            $p_attrib = &$this->m_attribList[$attrkeys[$i]];
            $s_record[$attrkeys[$i]] = &$p_attrib->fetchValue($r_record);
          }
        }

        // Load the initial values on intial loading
        else
        {
          // initial loading. We'll put the fields to reasonable defaults.
          $s_record = $this->initial_values();
        }
      }

      // Return the filter record
      return $s_record;
    }

    function getFiltersTable($filterrecord)
    {
      $theme = &atkinstance("atk.ui.atktheme");
      $reqimg = '<img align="top" src="'.$theme->imgPath("required_field.gif").'" border="0"
                  alt="'.atktext("field_obligatory").'" title="'.atktext("field_obligatory").'">';
      $filters = array();
      foreach ($this->m_attribIndexList as $id=>$attr)
      {
        $name = $attr["name"];
        $attr = &$this->m_attribList[$name];
        $label = $attr->getLabel();
        if ($attr->hasFlag(AF_OBLIGATORY))
          $label .= " $reqimg";
        if ($attr->hasFlag(AF_READONLY))
          $field = $attr->display($filterrecord).$attr->hide($filterrecord);
        else
          $field = $attr->edit($filterrecord);
        $filters[] = array($label, $field);
      }
      return $filters;
    }

    function getParameterBar($filterrecord)
    {
      $output = '<form name="entryform" id="entryform" action="'.dispatch_url($this->m_module.".".$this->m_type, "report").'" method="post" style="margin: 0px">';
      $output.= session_form();
      $output.= '<input type="hidden" name="go" value="1">'; //trigger to prevent loading the first time

      $buttons = array();
      $buttons[] = '<input type="submit" value="'.atktext("refresh").'">';
      $buttons[] = '<input type="button" name="printbutton" value="'.atkText("print").'" onClick=\'if (window.print) {var frame = parent.frames.main;if (frame) {frame.focus();frame.print();}}return false;\'>';

      $tbl = &atknew("atk.utils.atktablerenderer");
      $filterstable = $this->getFiltersTable($filterrecord);
      $filterstable[] = array('', implode(' ', $buttons));

      $output.= $tbl->render($filterstable);
      $output.= '</form>';

      return $output;
    }

    /**
     * Set up default values for this node
     *
     * @return array Array containing Attributename=>Initialvalue pairs
     */
    function initial_values()
    {
      return array();
    }

    function renderReportPage($parameterbar, $contentelements)
    {
      // Get the page and ui
      $page = &$this->getPage();
      $ui = &$this->getUi();

      // Register the stylesheet
      $this->addStyle("style.css");
      $page->register_style(moduleDir("reports")."style/printablereport.css", "print");
      $page->register_script(atkconfig("atkroot")."atk/javascript/tools.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/formsubmit.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/dhtml_formtools.js");

      // Render a box around all elements and put them in an array
      $boxedblocks = array();
      $boxedblocks[] = '<div class="contentblock">'.$ui->renderBox(array("title"=>$this->text('parameters'), "content"=>'<div class="content">'.$parameterbar.'</div>')).'</div>';
      foreach ($contentelements as $titlekey => $content)
      {
        $boxedblocks[] = '<div class="contentblock">'.$ui->renderBox(array("title"=>$titlekey, "content"=>'<div class="content">'.$content.'</div>')).'</div>';
      }

      // Merge the boxed blocks into an actionpage template and add the result to the page
      $actionpagecontent = $ui->render("actionpage.tpl", array("blocks"=>$boxedblocks, "title"=>$ui->title($this->m_module, $this->m_type, "report")));
      $page->addContent($actionpagecontent);
    }

    /**
     * Enter description here...
     *
     */
    function action_report()
    {
      // Get the filterrecod
      $filterrecord = $this->getFilterRecord();

      // If isgo, then validate the filter record
      if ($this->isGo())
        $this->validate($filterrecord, "report");

      // Create the filterbar and content html
      $parameterbar = $this->getParameterBar($filterrecord);
      if (!$this->isGo() || (count(atkArrayNvl($filterrecord, "atkerror"))>0))
        $contentelements = array();
      else
        $contentelements = $this->getContentElements($filterrecord);

      // Render the page
      $this->renderReportPage($parameterbar, $contentelements);
    }

    /**
     * Abstract method, should be overridden to create the html content
     *
     * @param unknown_type $filterrecord
     */
    function getContentElements($filterrecord)
    {
      atkerror("modules.reports.basereport->getContentElements needs to be overridden to generate content");
      return array();
    }

  }

?>