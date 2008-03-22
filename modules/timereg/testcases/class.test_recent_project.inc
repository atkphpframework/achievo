<?php
class test_recent_project extends atkTestCase  
{
  /**
   * Inside the constructor we register the fixtures we
   * want to use inside this test-case. 
   */
  public function __construct()
  {
    parent::atkTestCase();
    
    $this->addFixture('timereg.person');
    $this->addFixture('timereg.hours');
    $this->addFixture('timereg.project');
    $this->addFixture('timereg.phase');
    $this->addFixture('timereg.project_person');
    $this->addFixture('timereg.accessright');
  }
  
  public function setUp()
  {
    parent::setUp();
    
    atkimport("atk.security.atkmocksecuritymanager");
    $this->setMockSecurityManager(new atkMockSecurityManager());
  }
  
  public function tearDown()
  {
    parent::tearDown();
    $this->restoreSecurityManager();  
  }
  
  /**
   * Test if there are only two employees in the database.
   */
  public function test_recentProject()
  {
    useattrib("project.projectmanytoonerelation");
    atkimport("module.project.project");
    $project = &new projectManyToOneRelation("projectid","project.project");
    
    $mockmgr = &atkGetSecurityManager();
      
    $mockmgr->setAllowed(true);
    $project->setUserId(1); //

    $ids = $this->filter($project->getRecentProjectsIds());
    $this->assertEqual($ids, array(19,28,27,26,25,24,23,22,21,20));

    $ids = $this->filter($project->getRecentProjectsIds(28));
    $this->assertEqual($ids, array(19,28,27,26,25,24,23,22,21,20));

    $ids = $this->filter($project->getRecentProjectsIds(30));
    $this->assertEqual($ids, array(30,19,28,27,26,25,24,23,22,21,20));
    
    $project->setUserId(2); 
    $mockmgr->setAllowed(false);

    $ids = $this->filter($project->getRecentProjectsIds());
    $this->assertEqual($ids, array(20,19));

    $ids = $this->filter($project->getRecentProjectsIds(30));
    $this->assertEqual($ids, array(30,20,19));
  }
  
  function filter($arr)
  {
    $res =  array();
    foreach ($arr as $key=>$value)
    {
      $res[] = $value['id'];
    }
    return $res;
  }

}
?>