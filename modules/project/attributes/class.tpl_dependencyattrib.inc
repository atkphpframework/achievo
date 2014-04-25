<?php
/**
 * This file is part of the Achievo distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package achievo
 * @subpackage project
 *
 * @copyright (c)2008 Ibuildings B.V.
 * @license http://www.achievo.org/licensing Achievo Open Source License
 *
 * @version $Revision$
 * $Id$
 */
useattrib("project.dependencyattrib");

/**
 * Phase dependency attribute
 * 
 * @package achievo
 * @subpackage project
 */
class tpl_dependencyAttribute extends dependencyAttribute
{

    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $flags Flags for this attribute
     */
    function tpl_dependencyAttribute($name, $flags = 0)
    {
        /* base class constructor */
        $this->dependencyAttribute($name, $flags);
    }

    /**
     * Get phases of a project
     *
     * @param int $projectid Project id
     * @return array
     */
    function getPhases($projectid)
    {
        $db = &atkGetDb();

        $query = "SELECT
                id, name
              FROM
                tpl_phase, tpl_project_phase
              WHERE
                id = phaseid
                AND projectid = " . $projectid . " ORDER BY name";

        $recs = $db->getrows($query);
        return $recs;
    }

    /**
     * Load function
     *
     * @param mixed $notused Not used
     * @param array $rec Record
     * @return array
     */
    function load($notused, $rec)
    {
        $query = "SELECT
                phaseid_row as first,
                ROW1.name as firstname,
                phaseid_col as last,
                COL1.name as lastname
              FROM
                tpl_dependency, tpl_phase ROW1, tpl_phase COL1
              WHERE
                projectid = " . $rec["id"] . "
                AND ROW1.id = phaseid_row
                AND COL1.id = phaseid_col
                ORDER BY phaseid_row";

        $db = &atkGetDb();
        return array("deps" => $db->getrows($query));
    }

    /**
     * Store
     *
     * @param mixed $notused Not used
     * @param array $rec Record
     * @return boolean
     */
    function store($notused, $rec)
    {
        $db = &atkGetDb();

        $values = $rec[$this->fieldName()];

        // First delete deleted items.
        $delarr = $values["del"];
        if (is_array($delarr)) {
            for ($i = 0; $i < count($delarr); $i++) {
                $delquery = "DELETE
                     FROM
                       tpl_dependency
                     WHERE projectid = " . $rec["id"] . "
                       AND phaseid_row = " . $delarr[$i]["first"] . "
                       AND phaseid_col = " . $delarr[$i]["last"];

                $db->query($delquery);
            }
        }

        // Now add a new item..
        if ($values["first"] != "" && $values["last"] != "") {
            $query = "INSERT
                  INTO tpl_dependency (projectid,phaseid_row,phaseid_col)
                  VALUES (" . $rec["id"] . "," . $values["first"] . "," . $values["last"] . ")";

            $db->query($query);
        }
        return true;
    }

}

?>