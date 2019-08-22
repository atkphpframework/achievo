> :exclamation: :exclamation: **END OF LIFE NOTICE** :exclamation: :exclamation:
>
> Development of Achieve has stopped a very long time ago. This repository exists for historical reasons only and is thus archived. We highly discourage using Achievo now. **Achievo has known security issues!** It won't work with recent PHP versions either. Please consider switching to another BSS application.
>
> If you're not looking for Achievo, but the ATK Framework it is based on, we got good news for you. [@Sintattica](https://github.com/Sintattica/atk) took over development and is still supporting it as an Open Source project. We highly recommend switching to their code base.
>
> :exclamation: :exclamation: **END OF LIFE NOTICE** :exclamation: :exclamation:


Achievo
=======
Achievo is a web-based Business Support Services (BSS) application for organizations, built using [the ATK Framework](https://www.github.com/atkphpframework/atk). Achievo includes a rich set of core modules, including support for employees, projects (phases and activities), timesheets, organizations, contacts, CRM (customers, campaings and contracts), and document management. There are a wide variety of Add-on modules available for additional functionality, and you can add or develop Custom modules for more more specific requirements. Achievo is stable and suitable for production. For more information, see the [Achievo GitHub project wiki](https://github.com/atkphpframework/achievo/wiki) and the [Achievo website](http://www.achievo.org).

The purpose of the [Achievo project on GitHub](https://www.github.com/atkphpframework/achievo) is to create, as a community, the leading rapid-development environment for providing enterprise Business Support Services. Achievo was created by [ibuildings.nl](http://www.ibuildings.nl) and development is now carried on by the Achievo community, managed by the [GitHub atkphpframework group](https://www.github.com/atkphpframework/), with the endorsement and permission of [ibuildings.nl](http://www.ibuildings.nl).

* Achievo is licensed under the terms of the GNU GPL v2. See the doc/COPYRIGHT and DOC/LICENSE files, and the [License page in the Achievo project wiki](https://github.com/atkphpframework/achievo/wiki/License) for more information.

* Achievo is a trademark of ibuildings.nl. See the [Achievo Trademark and Logo Policy](https://github.com/atkphpframework/achievo/wiki/Trademark-and-Logo-Policy) for more information.

To Get Started
==============
Achievo can be hosted on a standard web application AMP stack (Apache/MySQL/PHP), and is compatible with current AMP stack versions (e.g. PHP 5.4.x and MySQL 5.5.x, although currently Achievo makes use of some deprecated PHP features).

The most convenient way to get started is to clone the [Achievo GitHub repository](https://github.com/atkphpframework/achievo) to your webserver.

<code>
$ git clone --recurse-submodules git://github.com/atkphpframework/achievo.git achievo
</code>

After cloning, the local master branch in the repo will contain the most recent release (i.e. deploy the master branch to run the lastest release). For most people, checkout the develop branch to develop with Achieveo, and include commits from developers made since the last release. 

Achievo is built using the ATK Framework and includes ATK as a submodule; the --recurse-submodules option tells Git to update the ATK submodule during the cloning, otherwise the atk/ directory will be empty and you will have to update submodules manually). You do not need a GitHub account to clone using the "git:" protocol.

You can also download a zip archive of the [Achievo master branch](https://github.com/atkphpframework/achievo) and extract it to your webserver. However, in this case you will also need to download a zip archive of version 6.4.4 of the ATK Framework and extract it to your achievo/atk/ directory (currently from https://github.com/dalers/atk-6.4.4, although this will eventually change to the GitHub  [ATK Framework project](https://github.com/atkphpframework/atk).

Once you have Achievo on your server or workstation, review the doc/INSTALL and doc/README files. These should explain everything you need to complete the install. Also review the [Guided Tour](http://github.com/atkphpframework/achievo/wiki/Achievo-guided-tour) on the Achievo project wiki, which is a step-by-step tutorial for configuring a new Achievo installation and using Achievo's basic features.

Version Control and Issue Tracking
==================================
* [Achievo GitHub project canonical repository](https://github.com/atkphpframework/achievo)
* [Achievo GitHub project issue tracker](https://github.com/atkphpframework/achievo/issues)
* There is also the [ibuildings legacy Achievo and ATK bug tracker](http://bugzilla.achievo.org/query.cgi) for searching historical issues prior to Achievo 1.4.5  (if you find an issue here that applies to current development, please copy the pertinent details into a new GitHub issue and include a reference to the Bugzilla entry). 

Communication Channels
======================
Achievo users and developers discuss problems and solutions, keep each other informed, and generally help each other out, on the [Achievo/ATK forum](http://forum.achievo.org/). Please consider creating a free user profile on the forum; you will need a user profile to post or reply, (viewing does not require a profile), having a profile also enables you to be sent an email notice when someone replies to one of your posts (or creates a new post), and color-codes posts to show the ones you haven't read yet.

If you create a new forum topic, be careful to submit it to the Achivo forum (generally use the ATK forum only when you are developing an Achievo module and you have a low-level issue relating to the ATK Framework).

Developer Guidelines
====================
The Achievo project follows the Gitflow workflow using the Gitflow Fork & Pull model:
* Fork the GitHub atkphpframework/achievo repo
* Clone your fork locally
* Checkout the develop branch to work in, or create a local topic branch from either the develop branch or a release branch.
* Develop and test your work
* Push your topic branch to your GitHub clone
* Issue a pull request to the atkframework group to have your changes merged, typically into the develop branch for on-going development.

For more information, see [Developer Guidelines](https://github.com/atkphpframework/achievo/wiki/Developer-Guidelines) in the [GitHub Achievo project wiki](https://github.com/atkphpframework/achievo/wiki).

Repository Structure
--------------------
The Achievo project on GitHub was migrated from the ibuildings Subversion server at Achievo version 1.4.5. The current master branch branch essentially starts with the 1.4.5 release and includes 1.4.5 and later releases, and the develop branch also starts with the 1.4.5 release.

The svn-master branch includes some commits made after the 1.4.5 release, which can be reviewed for merging or cherry picking into the develop (or other branch).

    <==== ibuildings svn repository ====================================>
         br1  ...    brn       trunk
          |           |           |
          |           |           |
    <==== GitHub atkphpframework/achievo repository ====================>
          |           |           |
       svn-br1 ... svn-brn    svn-master     release-1.4  master   develop
                                                |           |        |
                            tag release_1_4_5-->|           |        |
                                                |-------->1.4.5      |
                                                |           |------->|
                                                |           |        |
                                                |-------->1.4.6      |
                                                |           |------> |
                                                |           |        |
    <==== NEW DEVELOPMENT GOES IN develop BRANCH ======================>
                                                |           |        |
                                                |           |<-x.y.z-|
                                                |           |        |
                                                |           |        |
                                                V           V        V

Note - "svn-br1 ... svn-brn" means the many other branches in the svn repo besides trunk. I tried using "svn-branch(1) ... svn-branch(n)", but the diagram became too wide.

=============
Official project documentation (although still somewhat sparse) is in the [Achievo GitHub project wiki](https://github.com/atkphpframework/achievo/wiki/), with valuable discussion topics in the [Achievo/ATK forum](http://forum.achievo.org/). The [Achievo website](http://www.achievo.org/) and [ATK Framework website](http://www.atk-framework.com/) are also valuable resources, as is the [ibuildings Achievo/ATK wiki](http://www.achievo.org/wiki/) (but may not be current, updated articles are published on the [Achievo GitHub project wiki](https://github.com/atkphpframework/achievo/wiki/)). The ATK Framework (used by Achievo) is  documented internally with PHPDoc comments, and the [ATK Framework API documentation](http://www.atk-framework.com/documentation/) can also be browsed on the [ATK Framework website](http://www.atk-framework.com/).

* [Achievo GitHub project wiki](https://github.com/atkphpframework/achievo/wiki/) 
* [Achievo/ATK forum](http://forum.achievo.org/)
* [Achievo website](http://www.achievo.org/)
* [ATK Framework website](http://www.atk-framework.com/)
* [ibuildings Achievo/ATK wiki](http://www.achievo.org/wiki/)

Support
=======
In the event you have a problem:

* Review Achievo behavior using an un-modified Achievo installation.
* Search the [Achievo GitHub project wiki](https://github.com/atkphpframework/achievo/wiki/) and the [ibuildings Achievo/ATK wiki](http://www.achievo.org/wiki/).
* Search the [Achievo/ATK forum](http://forum.achievo.org/).
* Search the [Achievo GitHub project Issues](https://github.com/atkphpframework/achievo/issues) and [ibuildings legacy bug tracker](http://bugzilla.achievo.org/query.cgi).
* Post a question to the [Achievo/ATK forum](http://forum.achievo.org/) (free account sign-up is required for posting).

To get the most informed response when posting to the forum try to include:

* What are you trying to achieve?
* What are the symptoms of your problem? Why do you think there *is* a problem?
* What is your server (OS, web server, PHP and MySQL)? E.g. FreeBSD 9.1, Apache 2.2.22, PHP 5.3.10 and MySQL 5.5.20.
* Include example code (the *simplest* code that demonstrates the symptoms).

