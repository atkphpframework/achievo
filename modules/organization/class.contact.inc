<?php

userelation("atkmanytoonerelation");
include_once (moduleDir("person")."/class.person.inc");

class contact extends person
{
  function contact()
  {
    $this->person("contact");
    $this->addFilter("person.role='contact'");
    $this->add(new atkManyToOneRelation("company","organization.organization",AF_HIDE));
    $this->add(new atkAttribute("role"           , AF_READONLY|AF_HIDE));
  }

  function initial_values()
  {
    $initial["role"] = "contact";
    return $initial;
  }

  function descriptor_def()
  {
    return "[lastname], [firstname]";
  }

  function action_view()
  {
    // view not only contactperson information but also meetings (including attendees of meetings) and projects
    global $g_layout, $g_db, $g_theme;

    list($field, $personid2) = explode("=", $this->m_postvars["atkselector"]);
    list($dummy, $personid, $dummy2) = explode("'", $personid2);
    $query = "SELECT * FROM person WHERE id=".$personid;
    $contact = $g_db->getRows($query);

    $query = "SELECT schedule.*, schedule_type.description as scheduletype FROM schedule
              LEFT JOIN schedule_attendee ON schedule_attendee.schedule_id = schedule.id
              LEFT JOIN schedule_type ON schedule.scheduletype = schedule_type.id
              WHERE schedule_attendee.person_id =".$personid;
    $appointments = $g_db->getRows($query);

    $g_layout->initGUI();
    $g_layout->ui_top(text("contact").' - '.text("view"));
    $g_layout->output($this->statusbar());
    $g_layout->output('<br>');

    $g_layout->table_simple();
    $g_layout->output('<tr>');
    $g_layout->td(text("lastname").": ");
    $g_layout->td($contact[0]["lastname"]);
    $g_layout->output('</tr><tr>');
    $g_layout->td(text("firstname").": ");
    $g_layout->td($contact[0]["firstname"]);
    $g_layout->output('</tr><tr>');
    $g_layout->td(text("address").": ");
    $g_layout->td($contact[0]["address"]);
    $g_layout->output('</tr><tr>');
    $g_layout->td(text("zipcode").": ");
    $g_layout->td($contact[0]["zipcode"]);
    $g_layout->output('</tr><tr>');
    $g_layout->td(text("city").": ");
    $g_layout->td($contact[0]["city"]);
    $g_layout->output('</tr><tr>');
    $g_layout->td(text("country").": ");
    $g_layout->td($contact[0]["country"]);
    $g_layout->output('</tr><tr>');
    $g_layout->td(text("phone").": ");
    $g_layout->td($contact[0]["phone"]);
    $g_layout->output('</tr><tr>');
    $g_layout->td(text("cellular").": ");
    $g_layout->td($contact[0]["cellular"]);
    $g_layout->output('</tr><tr>');
    $g_layout->td(text("fax").": ");
    $g_layout->td($contact[0]["fax"]);
    $g_layout->output('</tr><tr>');
    $g_layout->td(text("email").": ");
    $g_layout->td('<a href="mailto:'.$contact[0]["email"].'">'.$contact[0]["email"]."</a>");
    $g_layout->output('</tr><tr>');
    $g_layout->td(text("function").": ");
    $g_layout->td($contact[0]["function"]);
    $g_layout->output('</tr><tr>');
    $g_layout->td(text("remark").": ");
    $g_layout->td($contact[0]["remark"]);

    $g_layout->output('</tr><tr>');
    $g_layout->td(text("appointments").": ", 'valign="top"');
    $g_layout->output('<td class="table" colspan="1" valign="top"><table border="0" cellspacing="0" cellpadding="0"><tr>');
    $g_layout->output('<td align="left"><table border="0" cellspacing="0" cellpadding="0" class="backtable"><tr>');
    $g_layout->output('<td><table border="0" cellspacing="1" cellpadding="3"><tr>');
    $g_layout->output('<td class="tableheader" >'.text("startdate").'</td>');
    $g_layout->output('<td class="tableheader" >'.text("enddate").'</td>');
    $g_layout->output('<td class="tableheader" >'.text("starttime").'</td>');
    $g_layout->output('<td class="tableheader" >'.text("endtime").'</td>');
    $g_layout->output('<td class="tableheader" >'.text("title").'</td>');
    $g_layout->output('<td class="tableheader" >'.text("type").'</td>');
    $g_layout->output('<td class="tableheader" >&nbsp;</td></tr>');

    for ($i=0;$i<count($appointments);$i++)
    {
      $method = "rowColor";
      $bgh = $g_theme["RowMouseOver"];
      if (method_exists($this, $method))
      {
        $bgn = $this->$method($appointments[$i], $i);
        if (is_array($bgn)) list($bgn, $bgh) = $bgn;
      }

      /* alternate colors of rows */
      $g_layout->output('<tr class="row'.($i % 2 == 0 ? 1 : 2).'" '.(empty($bgn) ? '' : 'style="background-color:'.$bgn.'" ').
                   '  onmouseover="if (typeof(this.style) != \'undefined\') this.style.backgroundColor = \''.$bgh.'\'"'.
                   '  onmouseout="if (typeof(this.style) != \'undefined\') this.style.backgroundColor = \''.(empty($bgn)?'':$bgn).'\'">');
      $g_layout->output('<td class="table" >'.$appointments[$i]["startdate"].'</td>');
      $g_layout->output('<td class="table" >'.$appointments[$i]["enddate"].'</td>');
      $g_layout->output('<td class="table" >'.$appointments[$i]["starttime"].'</td>');
      $g_layout->output('<td class="table" >'.$appointments[$i]["endtime"].'</td>');
      $g_layout->output('<td class="table" >'.$appointments[$i]["title"].'</td>');
      $g_layout->output('<td class="table" >'.$appointments[$i]["scheduletype"].'</td>');
      $g_layout->output('<td class="table" ><a href="'.session_url(dispatch_url("calendar.schedule", "view", array("atkselector" => "schedule.id=".$appointments[$i]["id"])),SESSION_NESTED).'">'.text("view").'</td></tr>');
    }

    $g_layout->output('</table></td></tr></table></td></tr></table>');

    $node = &getNode("project.project_personcontact");
    $node->m_attribList["personid"]->m_flags |= AF_HIDE_LIST;
    $node->m_attribList["projectid"]->m_flags = $node->m_attribList["projectid"]->m_flags&(~AF_HIDE);
    $data = $node->selectDb("personid=".$personid);
    $g_layout->output('</tr><tr>');
    $g_layout->td(text("projects").": ", 'valign="top"');
    if (count($data) != 0) $g_layout->td($node->recordList($data, array(), false)); else $g_layout->td(text("none"));

    $g_layout->output('<br></tr></table>');

    $g_layout->output('<br><form method="get">');
    $g_layout->output(session_form(SESSION_BACK));
    $g_layout->output('<input type="submit" value="&lt;&lt; '.text("back").'"></form>');
    $g_layout->ui_bottom();
  }
}

?>
