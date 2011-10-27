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
   | Authors: Pierre <pierre@php.net>                                     |
   +----------------------------------------------------------------------+
   $Id: index.php 317355 2011-09-26 21:08:47Z pajoye $
*/

$recent_release = release::getRecent();

$page = new PeclPage();
$page->title = 'PECL :: The PHP Extension Community Library ';
$page->setTemplate( PECL_TEMPLATE_DIR . '/index.html');



if (count($recent_release)) {
    $data = array('recent_release' => $recent_release);
    $page->addData($data);
}

$page->render();

echo $page->html;