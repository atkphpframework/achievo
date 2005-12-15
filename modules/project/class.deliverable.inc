<?php

  useattrib("atkdateattribute");
  useattrib("atklistattribute");
  useattrib("atknumberattribute");
  useattrib("atkboolattribute");
  userelation("atkmanytoonerelation");

  /**
   * The node class for deliverable.
   */
  class deliverable extends atkNode
  {
    function deliverable()
    {
      $this->atkNode("deliverable");
      $this->add(new atkAttribute("id", AF_AUTOKEY));

      $this->add(new atkAttribute("name", AF_OBLIGATORY, 50));
      $this->add(new atkManyToOneRelation("project_id", "project.project", AF_HIDE|AF_OBLIGATORY));
      $this->add(new atkDateAttribute("duedate", AF_OBLIGATORY));
      $this->add(new atkListAttribute("status", array("new","complete"), array(), AF_HIDE_ADD|AF_OBLIGATORY|AF_FORCE_LOAD));
      $this->add(new atkNumberAttribute("remind_days"));

      // Tracking
      $this->add(new atkManyToOneRelation("userid","employee.employee", AF_HIDE|AF_FORCE_LOAD|AF_OBLIGATORY|AF_READONLY));
      $this->add(new atkDateAttribute("entrydate",AF_HIDE|AF_READONLY|AF_OBLIGATORY));

      $this->add(new atkNumberAttribute("emailstatus", AF_HIDE|AF_FORCE_LOAD));

      $this->setTable("deliverable");
      $this->setOrder("duedate", "name");
    }

    /**
     * Set up default values for this node
     * @return array Associative array where the key is
     *               the attribute name, and value is the
     *               default value.
     */
    function initial_values()
    {
      $user = &getUser();
      return array("userid"=>array("id"=>$user["id"]),
                   "remind_days"=>7,
                   "status"=>"new",
                   "entrydate"=>array("year"=>date("Y"),
                     "month"=>date("m"),
                     "day"=>date("d")));
    }

    /**
     * Return record descriptor (for use in relations etc.)
     * @return String A template containing fieldnames between brackets.
     */
    function descriptor_def()
    {
      return "[name]";
    }

    function rowColor($record)
    {
      $duedate = dateutil::arr2str($record["duedate"]);

      if ($record["status"]=="new")
      {
        if ($duedate!=""&&$duedate<date("Ymd"))
        {
          return COLOR_ERROR;
        }
        else if ($duedate!=""&&dateutil::date_diff(date("Ymd"), $duedate)<$record["remind_days"])
        {
          return COLOR_WARNING;
        }
      }
      else if ($record["status"]=="complete")
      {
        return COLOR_OK;
      }
    }
  }

?>