<?php

  /**
   * Todo history, to keep track of changes to todo's.
   *
   * @author Dan Frame <dan_frame@jabil.com>
   *         
   * Rewrite to be an extension of todo class by Ivo.
   *
   * @version $Revision$
   *
   * $Id$
   *
   */

  require_once("achievotools.inc");
  require_once("class.todo.inc");
  useattrib("atkdummyattribute");  

  class todo_history extends todo
  { 
    function todo_history()
    {
      $this->todo(); // call baseclass constructor, but with new name.            
      
      $this->m_flags|= NF_NO_ADD|NF_NO_EDIT|NF_NO_DELETE;
      $this->add(new atkManyToOneRelation("todoid"    , "todo",AF_READONLY|AF_OBLIGATORY|AF_SEARCHABLE));
      
      // We disable the history attribute, for it will cause a recursive loop if history would have
      // a history.
      $this->remove("history");
      
      // We also need to change the updated attribute, because now we no longer want it hide_list,
      // and we don't want to store the current date, but the original date that the todo was
      // last changed before it ended up in the history.
      $this->remove("updated");
      $this->add(new atkAttribute("updated"));
      
      $this->setTable("todo_history", "todo_history");         
    
      // Ordering by id makes sure we sort in order of history.
      $this->setOrder("id");

    }
    
    function postUpdate($rec)
    {
      // do nothing. we need to define this dummy, otherwise todo::postUpdate
      // might be called, which creates another history item.
    }
    
    function postAdd($rec)
    {
      // do nothing. we need to define this dummy, otherwise todo::postAdd
      // might be called, which spams people about new todo's.
    }

  }
?>