<?php
/*
   +----------------------------------------------------------------------+
   | PEAR Web site version 1.0                                            |
   +----------------------------------------------------------------------+
   | Copyright (c) 2003 The PEAR Group                                    |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Martin Jansen <mj@php.net>                                  |
   +----------------------------------------------------------------------+
   $Id$
*/
response_header("Privacy Policy");
?>

<h1>Privacy Policy</h1>

<h2>Email</h2>

<p>Package maintainer and bug reporter email addresses are shown slightly obfuscated.
Unobfuscated email addresses are never given to anyone not related to the operations
of pecl.php.net. We will also <b>never</b> ask you to send us any of your passwords
via email.</p>

<h2>Logfiles</h2>

<p>We are keeping logfiles of the requests that reach our web servers,
but we do only use those files for statistical purposes.</p>

<h2>Cookies</h2>

<p>Pecl.php.net uses cookies to keep track of user sessions. Unless
you login on the site, the cookies will not be used to store personal
information and we do not give away the information from the cookies.</p>

<p><?php echo make_link("/about/", "Back"); ?></p>

<?php
response_footer();
?>
