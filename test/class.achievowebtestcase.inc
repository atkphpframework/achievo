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

  require_once(atkconfig("atkroot") . "atk/test/simpletest/web_tester.php");

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
     * Set the connection timeout at the start of each webtest
     *
     * Sets the connection timeout on http requests according to the
     * test_connectiontimeout as defined in atkconf.inc. This feature
     * can help avoid failing tests due to performance problems.
     */
    function setUp() {
      $this->setConnectionTimeout(atkconfig("test_connectiontimeout", 60));
    }

    /**
     * Logs the webtester in to Achievo
     *
     * Performs a login through the index page of the achievo webfolder. As
     * login data (username and password) the preconfigured test_username and
     * test_password variables in atkconf.inc are used.
     */
    function loginAchievo() {
      $this->get(atkconfig("test_achievourl"));
      $this->setField("auth_user", atkconfig("test_username"));
      $this->setField("auth_pw", atkconfig("test_password"));
      $this->clickSubmit(atktext("login", "", "", atkconfig("language")));
    }

    /**
     * Converts a relative URL to an achievo page to an absolute URL
     *
     * Converts a relative URL whithin the achievo root to an absolute URL.
     * To compile the absolute URL, the test_achievourl variable from
     * atkconf.inc is used.
     *
     * @param string Relative URL to file in achievo webroot
     * @return string Absolute URL
     */
    function achievoURL($url) {
      return atkconfig("test_achievourl") . $url;
    }

  }
?>