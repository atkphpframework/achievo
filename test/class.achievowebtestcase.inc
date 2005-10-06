<?php
  require_once(atkconfig("atkroot")."atk/test/simpletest/web_tester.php");
  
  class achievoWebTestCase extends WebTestCase {

    /**
     *    Sets up unit test wide variables at the start
     *    of each test method. To be overridden in
     *    actual user test cases.
     *    @access public
     */
    function setUp() {
    }

    /**
     *    Clears the data set in the setUp() method call.
     *    To be overridden by the user in actual user test cases.
     *    @access public
     */
    function tearDown() {
    }

    function loginAchievo() {
      global $config_achievourl, $config_achievousername, $config_achievopassword;
      $this->get($config_achievourl);
      $this->setField("auth_user", $config_achievousername);
      $this->setField("auth_pw", $config_achievopassword);
      $this->clickSubmit("Inloggen");
    }
    
    function achievoURL($url) {
      global $config_achievourl;
      return $config_achievourl . $url;
    }    
  }
?>