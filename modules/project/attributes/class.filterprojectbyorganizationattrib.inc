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
      
   useattrib("atkdummyattribute");    

  /**
    * If you add the following attribute to a node that has project information
    * in it (the project node itself, or for example the hoursurvey where project
    * information is loaded as well), then all data can be filtered by an
    * organizationid and a role. The role is optional, you can also filter just
    * by organization.
    *
    * Note that this is a filter-only attribute. It doesn't retrieve any extra
    * information (a feature I might add when needed), and doesn't display anything.
    *
    * @example: 
    *    $projectnode->add(new filterProjectByOrganizationAttrib(10, 2));
    *    $recs = $projectnode->selectDb();
    *    In this example, only records will be returned of projects
    *    where the organization with id 10 has performed role with id 2.
    *    (all projects where 'Graphic International' acted as 'Designer')    
    *
    * @author Ivo Jansch <ivo@ibuildings.nl>
    * @package achievo
    * @subpackage project
    */
  class filterProjectByOrganizationAttrib extends atkDummyAttribute
  {
    /**
     * Organization id
     * @var int
     */
    var $m_organizationid = NULL;
    /**
     * Project alias
     * @var string
     */
    var $m_projectalias = NULL;
    /**
     * Role id
     * @var int
     */
    var $m_roleid = NULL;
  
    /**
     * Constructor.
     *
     * @param int organizationid The id of the organization that you wish to 
     *                           filter by.
     * @param int roleid         The id of the role you wish to filter by. 
     *                           This parameter is optional. If you don't 
     *                           pass it, then all records are returned
     *                           that have an association with the specified
     *                           organization.
     * @param string projectalias This is a hard to understand parameter, 
     *                            which you usually don't need. This is what 
     *                            it does: the attribute filters projects by
     *                            organization, by hooking itself into the 
     *                            query that the node is going to perform.
     *                            In ordinary cases, the table it needs to 
     *                            join against is called 'project'. But in 
     *                            some occasions, project is present in the
     *                            query with an alias, for example when the
     *                            node does something like 
     *                            'select * from project A, hours B ....'
     *                            In this case, you need to tell the attribute
     *                            that it needs to join against 'A', so you
     *                            pass this as the third parameter.
     */
    function filterProjectByOrganizationAttrib($organizationid, $roleid="", $projectalias="")
    {
      $this->atkDummyAttribute("custfilter", "", AF_HIDE|AF_FORCE_LOAD);
      $this->m_organizationid = $organizationid;
      $this->m_projectalias = $projectalias;
      $this->m_roleid = $roleid;
    }
    
    /**
     * Add field to query
     *
     * @param atkQuery $query ATK Query object
     * @param string $table Tablename
     * @param string $aliasprefix Alias prefix
     * @param array $rec Record
     * @param int $level Level
     * @param string $mode Mode
     */
    function addToQuery(&$query, $table="", $aliasprefix="", $rec="", $level, $mode)
    {    
      if (!empty($this->m_organizationid))
      {        
        $query->addJoin("project_person", 
                        "projectmembers", 
                        (!empty($this->m_projectalias)?$this->m_projectalias:"project").".id = projectmembers.projectid".
                        (!empty($this->m_roleid)?" AND projectmembers.role=".$this->m_roleid:""), 
                        true);
        $query->addJoin("person", "person", "person.id = projectmembers.personid AND person.company='".$this->m_organizationid."'", false);      
        $query->setDistinct(true); // the join we perform could lead to duplicate records.
      }
    }
  } 

?>
