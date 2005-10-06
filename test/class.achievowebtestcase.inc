<?php
  /**
   * Achievo webtestcase class file
   *
   * The Achievo webtestcase class (inherited from SimpleTest's
   * WebTestCase), implements achievo-specific webtest features to
   * ease the development of webtests in Achievo.
   *
   * @package achievo
   * @subpackage test
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html  GNU General Public License 2
   *
   * @version $Revision$
   * $Id$
   */
  
  require_once(atkconfig("atkroot")."atk/test/simpletest/web_tester.php");
  
  /**
   * Class that handles userinterface internationalization.
   * 
   * Implements achievo-specific webtest features to ease the
   * development of webtests in Achievo. 
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo
   * @subpackage test
   *
   */
  class achievoWebTestCase extends WebTestCase {

    /**
     * Logs the webtester in to Achievo
     *
     * Performs a login through the index page of the achievo webfolder. As
     * login parameters the $config_achievousername and
     * $config_achievopassword variables from the test.php file in the
     * achievo root are used.
     */
    function loginAchievo() {
      global $config_achievourl, $config_achievousername, $config_achievopassword;
      $this->get($config_achievourl);
      $this->setField("auth_user", $config_achievousername);
      $this->setField("auth_pw", $config_achievopassword);
      $this->clickSubmit("Inloggen");
    }
    
    /**
     * Converts a relative URL to an achievo page to an absolute URL
     *
     * Converts a relative URL whithin the achievo root to an absolute URL.
     * To compile the absolute URL, the $config_achievourl variable from
     * test.php in the achievo root is used.
     *
     * @param string Relative URL to file in achievo webroot
     * @return string Absolute URL
     */
    function achievoURL($url) {
      global $config_achievourl;
      return $config_achievourl . $url;
    }
  
  }
?>