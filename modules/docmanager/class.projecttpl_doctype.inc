<?php
 /**
  * projecttpl_doctype node class file
  *
  * @package achievo_modules
  * @subpackage docmanager
  *
  * @author ivo <ivo@ibuildings.nl>
  * @author guido <guido@ibuildings.nl>
  *
  * @copyright (c) 2005 Ibuildings.nl BV
  * @license http://www.achievo.org/atk/licensing/ ATK open source license
  * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
  *
  * @version $Revision$
  * $Id$
  */

  /**
   * @internal includes
   */
  userelation("atkmanytoonerelation");

 /**
  * The node class for projecttpl_doctype.
  *
  * @author ivo <ivo@ibuildings.nl>
  * @author guido <guido@ibuildings.nl>
  * @package achievo_modules
  * @subpackage docmanager
  */
 class projecttpl_doctype extends atkNode
 {
   /**
    * projecttpl_doctype constructor
    */
   function projecttpl_doctype()
   {
     // Call the base class constructor
     $this->atkNode("projecttpl_doctype");

     // Add the relations to both the tpl_project and the documenttype tables
     $this->add(new atkManyToOneRelation("projecttpl_id", "project.tpl_project", AF_PRIMARY));
     $this->add(new atkManyToOneRelation("documenttype_id", "docmanager.documenttype", AF_PRIMARY));

     // Set the table for this node
     $this->setTable("docmanager_prjtpl_dt");
   }

 }

?>