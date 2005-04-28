<?  
class department extends atkNode
{
  function department()
  {
    global $g_securityManager;

    $this->atkNode("department",NF_AUTOSELECT|NF_EDITAFTERADD);

    $this->add(new atkAttribute("name",AF_PRIMARY));
    $this->add(new atkAttribute("description"));

    $this->setOrder("name");
    $this->setTable("employee_department");
  }
}
?>