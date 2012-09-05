<?php

  /**
   * Extension of the phase node, with added capabilities of filtering
   * phases. (status, etc.)
   */

  include_once(moduleDir("project")."class.phase.inc");

  class phaseselector extends phase
  {
    function phaseselector()
    {
      $this->phase("phaseselector", NF_READONLY);
      $this->setSecurityAlias("project.phase");
      $this->add(new phaseFilter());
    }

  }

  // @todo, this can be generalied into an 'activefilter', it's not really phase specific.
  class phaseFilter extends atkAttribute
  {
    function phaseFilter()
    {
      $this->atkAttribute("phasefilter", AF_HIDE|AF_FORCE_LOAD);
    }

    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      if ($mode != 'select') return;
      $query->addCondition("$tablename.status='active'");
    }
  }

?>