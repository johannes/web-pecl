<?php
/*
   +----------------------------------------------------------------------+
   | PEAR Web site version 1.0                                            |
   +----------------------------------------------------------------------+
   | Copyright (c) 2001-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   +----------------------------------------------------------------------+
   $Id: renum.php 317215 2011-09-23 20:05:18Z pajoye $
*/

require_once "pear-config.php";
require_once "pear-debug.php";
require_once "pear-database.php";
include PECL_INCLUDE_DIR . '/pear-database-category.php';

$dbh = DB::connect(PECL_DATABASE_DSN, array('persistent' => true));
renumber_visitations(true);
