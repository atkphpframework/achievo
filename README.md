Achievo
=======
Achievo is a web-based resource management application for organizations, built using the ATK Framework. Achievo includes a rich set of core modules, providing support for employees, projects (phases and activities), timesheets, organizations, contacts, CRM, document management and more. You can select additional modules from a wide selection of existing modules, and develop your own modules to add new functionality. Achievo is stable and suitable for production. For more information, see the [Achievo GitHub project wiki](https://github.com/atkphpframework/achievo/wiki) and the [Achievo website](http://www.achievo.org).

The purpose of the atkphpframework/Achievo project on GitHub is to create, as a community, the leading rapid-development environment for creating enterprise web applications. Achievo and the ATK Framework were created by [ibuildings.nl](http://www.ibuildings.nl) and development is now carried on by the open source community, managed by the [GitHub atkphpframework group](https://www.github.com/atkphpframework/), with the endorsement and permission of [ibuildings.nl](http://www.ibuildings.nl).

* Achievo is licensed under the terms of the GNU GPL v2. See the doc/COPYRIGHT and DOC/LICENSE files, and the [License page in the Achievo project wiki](https://github.com/atkphpframework/achievo/wiki/License) for more information.

* Achievo is a trademark of ibuildings.nl. See the [Trademark and Logo Policy in the Achievo project wiki](https://github.com/atkphpframework/achievo/wiki/Trademark-and-Logo-Policy) for more information.

To Get Started
==============
Achievo can be hosted on a standard web application AMP stack (Apache/MySQL/PHP), and is compatible with current AMP stack component versions (e.g. PHP 5.4.x, MySQL 5.5.x), although currently Achievo makes use of some deprecated PHP features.

The most convenient way to get started is to clone the [Achievo GitHub repository](https://github.com/atkphpframework/achievo) to your webserver. The master branch, containing the most recent production release, will be automatically checked out. Use the --recurse-submodules option for the ATK Framework submodule in Achievo to be automatically updated as part of cloning (if not, the atk/ directory will be empty and you will need to update submodules manually).

<code>
$ git clone --recurse-submodules git://github.com/atkphpframework/achievo.git achievo
</code>

Alternatively, you can download a zip archive of the [Achievo master branch](https://github.com/atkphpframework/achievo) and extract it to your webserver. However, in this case you must also separately download a zip archive of the ATK Framework and extract it to the achievo/atk/ directory (Achievo currently uses ATK 6.4.4, which is available from https://github.com/dalers/atk-6.4.4 until the GitHub atkphpframework/atk project has been fully setup).

Once you have the Achievo application on your server, review the doc/INSTALL and doc/README files. They should explain everything you need to complete the install. Also review the [Guided Tour](http://github.com/atkphpframework/achievo/wiki/Achievo-guided-tour) on the Achievo project wiki, which describes step-by-step how to configure a new Achievo installation for use, and how to use Achievo's most important features.

Version Control and Issue Tracking
==================================
* [Achievo GitHub project canonical repository](https://github.com/atkphpframework/achievo)
* [Achievo GitHub project issue tracker](https://github.com/atkphpframework/achievo/issues)
* [ibuildings Achievo/ATK bugzilla](http://bugzilla.achievo.org/query.cgi) for searching historical issues prior to Achievo 1.4.5  (if you find an issue that applies to the current release, please copy the pertinent details into a new GitHub issue and include a reference to the Bugzilla entry). 

Communication Channels
======================
Achievo users and developers discuss problems and solutions, keep each other informed, and generally help each other out, on the [Achievo/ATK forum](http://forum.achievo.org/). It's a good idea to create a free user profile on the forum, you will need it to post topics or reply to existing topics (although viewing does not require a profile). If you create a new topic, please be careful to submit it in an Achievo sub-forum (not an ATK Framework sub-forum).
* [Achievo/ATK forum](http://forum.achievo.org/)

Developer Guidelines
====================
See [Developer Guideines](https://github.com/atkphpframework/achievo/wiki/Developer-Guidelines) in the Achievo project wiki.

Documentation
=============
Official project documentation (although still somewhat sparse) is in the [Achievo GitHub project wiki](https://github.com/atkphpframework/achievo/wiki/), with valuable discussion topics in the [Achievo/ATK forum](http://forum.achievo.org/). The [Achievo website](http://www.achievo.org/) and [ATK Framework website](http://www.atk-framework.com/) are also valuable resources, as is the [ibuildings Achievo/ATK wiki](http://www.achievo.org/wiki/)(although detailed information may not be current, updated information is published on the [Achievo GitHub project wiki](https://github.com/atkphpframework/achievo/wiki/)). The ATK Framework (used by Achievo) is  documented internally with PHPDoc comments, and the [ATK Framework API documentation](http://www.atk-framework.com/documentation/) can also be browsed on the [ATK Framework website](http://www.atk-framework.com/).

* [Achievo GitHub project wiki](https://github.com/atkphpframework/achievo/wiki/) 
* [Achievo/ATK forum](http://forum.achievo.org/)
* [Achievo website](http://www.achievo.org/)
* [ATK Framework website](http://www.atk-framework.com/)
* [ibuildings Achievo/ATK wiki](http://www.achievo.org/wiki/)

Support
=======
In the event you have a problem:

* Review Achievo behavior using an unaltered production-release Achievo installation.
* Search the [Achievo GitHub project wiki](https://github.com/atkphpframework/achievo/wiki/) and the [ibuildings Achievo/ATK wiki](http://www.achievo.org/wiki/).
* Search the [Achievo/ATK forum](http://forum.achievo.org/).
* Search the [Achievo GitHub project Issues](https://github.com/atkphpframework/achievo/issues) and [ibuildings legacy bug tracker](http://bugzilla.achievo.org/query.cgi).
* Post a question to the [Achievo/ATK forum](http://forum.achievo.org/) (free account sign-up is required for posting). When posting to the forum, always include the following to help other Achievo users and developers give you the best response:
** a summary of what you are trying to achieve and symptoms of the problem (what you think is going wrong, or not happening when it should)
** your server operating system and version (e.g., FreeBSD 9.0, Ubunto 11.10, OpenSUSE 12.1, Windows 7, ...)
** your web server and version (e.g., Apache 2.2.22)
** your PHP version (e.g., PHP 5.3.10)
** your MySQL server version (e.g., MySQL 5.5.20)
** (optional) example code. Sometimes posting code is the simplest way to describe a technical problem you're having. If you are posting code, provide the simplest example that demonstrates your problem.

