<?php /* vim: set noet ts=4 sw=4 ft=php : */

/**
 * Contains functions and variables used throughout the bug system
 *
 * This source file is subject to version 3.0 of the PHP license,
 * that is bundled with this package in the file LICENSE, and is
 * available through the world-wide-web at the following URI:
 * http://www.php.net/license/3_0.txt.
 * If you did not receive a copy of the PHP license and are unable to
 * obtain it through the world-wide-web, please send a note to
 * license@php.net so we can mail you a copy immediately.
 *
 * @category  peclweb
 * @package   Bugs
 * @copyright Copyright (c) 1997-2004 The PHP Group
 * @license   http://www.php.net/license/3_0.txt  PHP License
 * @version   $Id$
 */


/*
 * DEFINE VARIABLES ============================
 */

// Have this style sheet included in all bug pages.
$extra_styles[] = '/css/bugs.css';

// For distinguishing this site from PEAR
$site = 'pecl';
$siteBig = 'PECL';
$bugEmail = 'pecl-dev@lists.php.net';

// used in mail_bug_updates(), below, and class for search results
$tla = array(
    'Open'        => 'Opn',
    'Bogus'       => 'Bgs',
    'Feedback'    => 'Fbk',
    'No Feedback' => 'NoF',
    'Wont fix'    => 'WFx',
    'Duplicate'   => 'Dup',
    'Critical'    => 'Ctl',
    'Assigned'    => 'Asn',
    'Analyzed'    => 'Ana',
    'Verified'    => 'Ver',
    'Suspended'   => 'Sus',
    'Closed'      => 'Csd',
);

$types = array(
    'Bug'                     => 'Bug',
    'Feature/Change Request'  => 'Req',
);

// Used in show_state_options()
$state_types =  array (
    'Open'         => 2,
    'Closed'       => 2,
    'Duplicate'    => 2,
    'Critical'     => 1,
    'Assigned'     => 1,
    'Not Assigned' => 1,
    'Analyzed'     => 1,
    'Verified'     => 1,
    'Suspended'    => 1,
    'Wont fix'     => 1,
    'No Feedback'  => 1,
    'Feedback'     => 1,
    'Old Feedback' => 0,
    'Stale'        => 0,
    'Fresh'        => 0,
    'Bogus'        => 2,
    'All'          => 0
);

// Items that need bug tracking but aren't real packages
$pseudo_pkgs = array(
    'Web Site',
    'Documentation',
    'Bug System',
);
if ($site == 'pear') {
    $pseudo_pkgs[] = 'PEPr';
}


/*
 * DEFINE FUNCTIONS ============================
 */

/**
 * Obfuscates email addresses to hinder spammer's spiders
 *
 * Turns "@" into character entities that get interpreted as "at" and
 * turns "." into character entities that get interpreted as "dot".
 *
 * @param string $txt     the email address to be obfuscated
 * @param string $format  how the output will be displayed ('html', 'text')
 *
 * @return string  the altered email address
 */
function spam_protect($txt, $format = 'html')
{
    if ($format == 'html') {
        $translate = array(
            '@' => ' &#x61;&#116; ',
            '.' => ' &#x64;&#111;&#x74; ',
        );
    } else {
        $translate = array(
            '@' => ' at ',
            '.' => ' dot ',
        );
    }
    return strtr($txt, $translate);
}

/**
 * Escape string so it can be used as HTML
 *
 * If magic_quotes_gpc is on, the slashes are stripped out
 * then the escaping is done.
 *
 * @param string $in  the string to be sanitized
 *
 * @return string  the sanitized string
 *
 * @see rinse(), escapeSQL(), oneof(), field(), txfield()
 */
function clean($in)
{
    return htmlspecialchars(get_magic_quotes_gpc() ? stripslashes($in) : $in);
}

/**
 * Remove slashes from a string if magic_quotes_gpc is on
 *
 * @param string $in  the string to be sanitized
 *
 * @return string  the sanitized string
 *
 * @see clean(), escapeSQL(), oneof(), field(), txfield()
 */
function rinse($in)
{
    return get_magic_quotes_gpc() ? stripslashes($in) : $in;
}

/**
 * Escape strings so they can be used as literals in queries
 *
 * If magic_quotes_gpc is on, the slashes are stripped out
 * then the escaping is done.
 *
 * @param string|array $in  the data to be sanitized.  If it's an array, each
 *                           element is sanitized.
 *
 * @return string|array  the sanitized data
 *
 * @see clean(), rinse(), oneof(), field(), txfield()
 */
function escapeSQL($in)
{
    global $dbh;
    if (is_array($in)) {
        $out = array();
        if (get_magic_quotes_gpc()) {
            foreach ($in as $key => $value) {
                $out[$key] = $dbh->escapeSimple(stripslashes($value));
            }
        } else {
            foreach ($in as $key => $value) {
                $out[$key] = $dbh->escapeSimple($value);
            }
        }
        return $out;
    } else {
        if (get_magic_quotes_gpc()) {
            return $dbh->escapeSimple(stripslashes($in));
        } else {
            return $dbh->escapeSimple($in);
        }
    }
}

/**
 * Goes through each variable submitted and returns the value
 * from the first variable which has a non-empty value
 *
 * Handy function for when you're dealing with user input or a default.
 *
 * @param mixed  as many variables as you wish to check
 *
 * @return mixed  the value, if any
 *
 * @see clean(), rinse(), escapeSQL(), field(), txfield()
 */
function oneof()
{
    foreach (func_get_args() as $arg) {
        if ($arg) {
            return $arg;
        }
    }
}

/**
 * Returns the data from the field requested and sanitizes
 * it for use as HTML
 *
 * If the data from a form submission exists, that is used.
 * But if that's not there, the info is obtained from the database.
 *
 * @param string $n  the name of the field to be looked for
 *
 * @return mixed  the data requested
 *
 * @see clean(), rinse(), escapeSQL(), oneof(), txfield()
 */
function field($n) {
    return oneof(clean($_POST['in'][$n]),
                 htmlspecialchars($GLOBALS['bug'][$n]));
}

/**
 * Returns the data from the field requested and sanitizes
 * it for use as plain text
 *
 * If the data from a form submission exists, that is used.
 * But if that's not there, the info is obtained from the database.
 *
 * @param string $n  the name of the field to be looked for
 *
 * @return mixed  the data requested
 *
 * @see clean(), rinse(), escapeSQL(), oneof(), field()
 */
function txfield($n)
{
    return oneof(rinse($_POST['in'][$n]),
                 $GLOBALS['bug'][$n]);
}

/**
 * Checks if the data submitted from the form is different than the
 * info in the database
 *
 * @param string $n  the name of the field to be examined
 *
 * @return bool  true if the data is different, false if it's the same
 */
function changed($n) {
    return $_POST['in'][$n]
        && (rinse(trim($_POST['in'][$n])) != trim($GLOBALS['bug'][$n]));
}

/**
 * Prints age <option>'s for use in a <select>
 *
 * @param string $current  the field's current value
 *
 * @return void
 */
function show_byage_options($current)
{
    $opts = array(
        '0'   => 'the beginning',
        '1'   => 'yesterday',
        '7'   => '7 days ago',
        '15'  => '15 days ago',
        '30'  => '30 days ago',
        '90'  => '90 days ago',
    );
    while (list($k,$v) = each($opts)) {
        echo "<option value=\"$k\"", ($current==$k ? ' selected="selected"' : ''),
             ">$v</option>\n";
    }
}

/**
 * Prints a list of <option>'s for use in a <select> element
 * asking how many bugs to display
 *
 * @param int $limit  the presently selected limit to be used as the default
 *
 * @return void
 */
function show_limit_options($limit = 30)
{
    for ($i = 10; $i < 100; $i += 10) {
        echo '<option value="' . $i . '"';
        if ($limit == $i) {
            echo ' selected="selected"';
        }
        echo ">$i bugs</option>\n";
    }

    echo '<option value="All"';
    if ($limit == 'All') {
        echo ' selected="selected"';
    }
    echo ">All</option>\n";
}

/**
 * Prints bug type <option>'s for use in a <select>
 *
 * Options include "Bug" and "Feature/Change Request."
 *
 * @param string $current  bug's current type
 * @param bool   $all      whether or not 'All' should be an option
 *
 * @retun void
 */
function show_type_options($current = 'Bug', $all = false)
{
    global $types;

    if ($all) {
        if (!$current) {
            $current = 'All';
        }
        echo '<option value="All"';
        if ($current == 'All') {
            echo ' selected="selected"';
        }
        echo ">All</option>\n";
    } elseif (!$current) {
        $current = 'Bug';
    }

    foreach ($types as $k => $v) {
        echo '<option value="' . $k . '"';
        if ($current == $k) {
            echo ' selected="selected"';
        }
        echo '>' . $k . '</option>' . "\n";
    }
}

/**
 * Prints bug state <option>'s for use in a <select> list
 *
 * @param string $state      the bug's present state
 * @param int    $user_mode  the 'edit' mode
 * @param string $default    the default value
 *
 * @return void
 */
function show_state_options($state, $user_mode = 0, $default = '')
{
    global $state_types;

    if (!$state && !$default) {
        $state = 'Open';
    } elseif (!$state) {
        $state = $default;
    }

    /* regular users can only pick states with type 2 for unclosed bugs */
    if ($state != 'All' && $state_types[$state] == 1 && $user_mode == 2) {
        /* If state was 'Feedback', set state to 'Open' automatically. */
        if ($state == 'Feedback' || $state == 'No Feedback') {
            echo "<option>Open</option>\n";
        } else {
            echo "<option>$state</option>\n";
        }
        if ($state != 'Bogus') {
            echo "<option>Closed</option>\n";
        }
    } else {
        foreach($state_types as $type => $mode) {
            if ($mode >= $user_mode) {
                echo '<option';
                if ($type == $state) {
                    echo ' selected="selected"';
                }
                echo ">$type</option>\n";
            }
        }
    }
}

/**
 * Prints bug resolution <option>'s for use in a <select> list
 *
 * @param string $current   the bug's present state
 * @param int    $expanded  whether or not a longer explanation should be
 *                          displayed
 *
 * @return void
 */
function show_reason_types($current = '', $expanded = 0)
{
	if ($expanded) {
        echo '<option value=""></option>' . "\n";
    }
	while (list($k,$v) = each($GLOBALS['RESOLVE_REASONS'])) {
		echo '<option value="' . $k . '"';
        if ($current==$k) {
            echo ' selected="selected"';
        }
		echo ">$v[desc]";
        if ($expanded) {
            echo " ($v[status])";
        }
        echo "</option>\n";
	}
}

/**
 * Prints PHP version number <option>'s for use in a <select> list
 *
 * @param string $current  the bug's current version number
 * @param string $default  a version number that should be the default
 *
 * @return void
 */
function show_version_options($current, $default = '')
{
    $versions = array(
        '4.3.10',
        '4.3.9',
        '4.3.8',
        '4.3.7',
        '4.3.6',
        '4.3.5',
        '4.3.4',
        '4.3.3',
        '4.3.2',
        '4.3.1',
        '4.3.0',
        '4.2.2',
        '4.2.1',
        '4.2.0',
        '4_3 CVS-'.date('Y-m-d').' (stable)',
        '5.0.3',
        '5.0.2',
        '5.0.1',
        '5.0.0',
        '5_0 CVS-'.date('Y-m-d').' (dev)',
        '5_1 CVS-'.date('Y-m-d').' (dev)',
        'Irrelevant'
    );
    $use = 0;

    echo '<option value="">--Please Select--</option>' . "\n";
    while (list(,$v) = each($versions)) {
        echo '<option';
        if ($current == $v) {
            echo ' selected="selected"';
        }
        echo '>' . $v . "</option>\n";
        if ($current == $v) {
            $use++;
        }
    }
    if (!$use && $current) {
        echo '<option selected="selected">' . $current . "</option>\n";
    }
    echo '<option value="earlier">Earlier? Upgrade first!</option>' . "\n";
}

/**
 * Prints package name <option>'s for use in a <select> list
 *
 * @param string $current   the bug's present state
 * @param int    $show_any  whether or not 'Any' should be an option.  'Any'
 *                          will only be printed if no $current value exists.
 * @param string $default   the default value
 *
 * @return void
 */
function show_types($current, $show_any, $default = '')
{
    global $site;
    static $bug_items;

    if (!isset($bug_items)) {
        $bug_items = array();
        $bug_items['Web Site']      = 'Web Site';
        $bug_items['Bug System']    = '&nbsp;&nbsp;&nbsp;Bug System';
        $bug_items['PEPr']          = '&nbsp;&nbsp;&nbsp;PEPr';
        $bug_items['Documentation'] = 'Documentation';

        $list = package::listAll(false);
        foreach ($list as $name => $data) {
            $bug_items[$name] = $name;
        }
    }

    $use = 0;

    if (!$current && !$default && !$show_any) {
        echo "<option value=\"none\">--Please Select--</option>\n";
    } elseif (!$current && $show_any == 1) {
        $current = 'Any';
    } elseif (!$current) {
        $current = $default;
    }

    foreach ($bug_items as $key => $value) {
        if ($show_any == 1 || $key != 'Any') {
            echo "<option value=\"$key\"";
            if ((is_array($current) && in_array($key, $current)) ||
                ($key == $current))
            {
                echo ' selected="selected"';
            }
            echo ">$value</option>\n";
            if ($key == $current) {
                $use++;
            }
        }
    }
    if (!$use && $current && !is_array($current)) {
        echo '<option selected="selected">' . $current . "</option>\n";
    }
}

/**
 * Prints a series of radio inputs to determine how the search
 * term should be looked for
 *
 * @param string $current   the users present selection
 *
 * @return void
 */
function show_boolean_options($current)
{
    $options = array('any', 'all', 'raw');
    while (list($val, $type) = each($options)) {
        echo '<input type="radio" name="boolean" value="', $val, '"';
        if ($val === $current) {
            echo ' checked="checked"';
        }
        echo " />$type&nbsp;\n";
    }
}

/**
 * Display errors or warnings as a <ul> inside a <div>
 *
 * Here's what happens depending on $in:
 *   + string: value is printed
 *   + array:  looped through and each value is printed.
 *             If array is empty, nothing is displayed.
 *             If a value contains a PEAR_Error object,
 *   + PEAR_Error: prints the value of getMessage() and getUserInfo()
 *                 if DEVBOX is true, otherwise prints data from getMessage().
 *
 * @param string|array|PEAR_Error $in  see long description
 * @param string $class  name of the HTML class for the <div> tag.
 *                        ("errors", "warnings")
 * @param string $head   string to be put above the message
 *
 * @return bool  true if errors were submitted, false if not
 */
function display_bug_error($in, $class = 'errors', $head = 'ERROR:')
{
    if (PEAR::isError($in)) {
        if (DEVBOX == true) {
            $in = array($in->getMessage() . '... ' . $in->getUserInfo());
        } else {
            $in = array($in->getMessage());
        }
    } elseif (!is_array($in)) {
        $in = array($in);
    } elseif (!count($in)) {
        return false;
    }

    echo '<div class="' . $class . '">' . $head . '<ul>';
    foreach ($in as $msg) {
        if (PEAR::isError($msg)) {
            if (DEVBOX == true) {
                $msg = $msg->getMessage() . '... ' . $msg->getUserInfo();
            } else {
                $msg = $msg->getMessage();
            }
        }
        echo '<li>' . htmlspecialchars($msg) . "</li>\n";
    }
    echo "</ul></div>\n";
    return true;
}

/**
 * Prints a message saying the action succeeded
 *
 * @param string $in  the string to be displayed
 *
 * @return void
 */
function display_bug_success($in)
{
    echo '<div class="success">' . $in . "</div>\n";
}

/**
 * Send an email notice about bug aditions and edits
 *
 * @param
 *
 * @return void
 */
function mail_bug_updates($bug, $in, $from, $ncomment, $edit = 1)
{
    global $tla, $types, $site, $siteBig;

    $text = array();
    $headers = array();

    /* Default addresses */
    list($mailto,$mailfrom) = get_package_mail(oneof($in['package_name'],$bug['package_name']));

    /* Get rid of slashes in bug status */
    $bug['status'] = rinse($bug['status']);

    $headers[] = array(" ID", $bug['id']);

    switch ($edit) {
    case 3:
        $from = spam_protect(rinse($in['commentemail']), 'text');
        $headers[] = array(" Comment by", $from);
        $from = "\"$from\" <$mailfrom>";
        break;
    case 2:
        $from = spam_protect(txfield('email'), 'text');
        $headers[] = array(" User updated by", $from);
        $from = "\"$from\" <$mailfrom>";
        break;
    default:
        $headers[] = array(" Updated by", $from);
    }

    if (changed('sdesc')) {
        $headers[] = array("-Summary", $bug['sdesc']);
    }

    $prefix = " ";
    if (changed('email')) {
        $headers[] = array("-Reported By", spam_protect($bug['email']), 'text');
        $prefix = "+";
    }
    if ($f = spam_protect(txfield('email'), 'text')) {
        $headers[] = array($prefix.'Reported By', $f);
    }

    $fields = array(
        'status'       => 'Status',
        'bug_type'     => 'Type',
        'package_name' => 'Package',
        'php_os'       => 'Operating System',
        'php_version'  => 'PHP Version',
        'assign'       => 'Assigned To'
    );

    foreach ($fields as $name => $desc) {
        $prefix = " ";
        if (changed($name)) {
            $headers[] = array("-$desc", $bug[$name]);
            $prefix = "+";
        }
        /* only fields that are set get added. */
        if ($f = txfield($name)) {
            $headers[] = array($prefix.$desc, $f);
        }
    }

    # make header output aligned
    $maxlength = 0;
    $actlength = 0;
    foreach ($headers as $v) {
        $actlength = strlen($v[0]) + 1;
        $maxlength = (($maxlength < $actlength) ? $actlength : $maxlength);
    }

    # align header content with headers (if a header contains
    # more than one line, wrap it intelligently)
    $header_text = "";
    $spaces = str_repeat(" ", $maxlength + 1);
    foreach ($headers as $v) {
        $hcontent = wordwrap($v[1], 72-$maxlength, "\n$spaces"); # wrap and indent
        $hcontent = rtrim($hcontent); # wordwrap may add spacer to last line
        $header_text .= str_pad($v[0] . ":", $maxlength) . " " . $hcontent . "\n";
    }

    if ($ncomment) {
        $text[] = " New Comment:\n\n".rinse($ncomment);
    }

    $text[] = get_old_comments($bug['id'], empty($ncomment));

    /* format mail so it looks nice, use 72 to make piners happy */
    $wrapped_text = wordwrap(join("\n",$text), 72);

    /* user text with attention, headers and previous messages */
    $user_text  = "ATTENTION! Do NOT reply to this email!\n" .
                  "To reply, use the web interface found at\n" .
                  "http://$site.php.net/bugs/bug.php?id=$bug[id]&edit=2\n\n\n" .
                  $header_text .
                  $wrapped_text;

    /* developer text with headers, previous messages, and edit link */
    $dev_text = 'Edit report at ' .
                "http://$site.php.net/bugs/bug.php?id=$bug[id]&edit=1\n\n" .
                $header_text .
                $wrapped_text .
                "\n-- \nEdit this bug report at " .
                "http://$site.php.net/bugs/bug.php?id=$bug[id]&edit=1\n";

    /* send mail if status was changed or there is a comment */
    if ($in[status] != $bug[status] || $ncomment != "") {

        if (isset($in['bug_type']) && $in['bug_type'] != $bug['bug_type']) {
            $subj = $types[$bug['bug_type']] . '->' . $types[$in['bug_type']];
        } else {
            $subj = $types[$bug['bug_type']];
        }

        $old_status = $bug['status'];
        $new_status = $bug['status'];

        if ($in[status] != $bug[status] && $edit != 3) {    /* status changed */
            $new_status = $in['status'];
            $subj .= " #{$bug['id']} [{$tla[$old_status]}->{$tla[$new_status]}]: ";
        } elseif ($edit == 3) {             /* comment */
            $subj .= " #{$bug['id']} [Com]: ";
        } else {                            /* status did not change and not comment */
            $subj .= " #{$bug['id']} [{$tla[$bug['status']]}]: ";
        }

        # the user gets sent mail with an envelope sender that ignores bounces
        if (DEVBOX == false) {
            @mail($bug[email],
                  "[$siteBig-BUG] " . rinse($subj) . txfield('sdesc'),
                  $user_text,
                  "From: $siteBig Bug Database <$mailfrom>\n".
                  "X-PHP-Bug: $bug[id]\n".
                  "In-Reply-To: <bug-$bug[id]@$site.php.net>",
                  "-fbounces-ignored@php.net");
            # but we go ahead and let the default sender get used for the list
            @mail($mailto,
                  "[$siteBig-BUG] " . rinse($subj) . txfield('sdesc'),
                  $dev_text,
                  "From: $from\n".
                  "X-PHP-Bug: $bug[id]\n".
                  "X-PHP-Type: "       . rinse((($edit != 3) ? $in['bug_type']     : $bug['bug_type']))    . "\n" .
                  "X-PHP-Version: "    . rinse((($edit != 3) ? $in['php_version']  : $bug['php_version'])) . "\n" .
                  "X-PHP-Category: "   . rinse((($edit != 3) ? $in['package_name'] : $bug['package_name']))    . "\n" .
                  "X-PHP-OS: "         . rinse((($edit != 3) ? $in['php_os']       : $bug['php_os']))      . "\n" .
                  "X-PHP-Status: "     . rinse($new_status) . "\n" .
                  "X-PHP-Old-Status: " . rinse($old_status) . "\n" .
                  "In-Reply-To: <bug-$bug[id]@$site.php.net>",
                  "-fpear-sys@php.net");
        }
    }

    /* if a developer assigns someone else, let that other person know about it */
    if ($edit == 1 && $in['assign'] && $in['assign'] != $bug['assign']) {

        $info = user::info($in['assign'], "email");
        $email = $info['email'];

        if ($email == $from) {
            return;
        }

        if (DEVBOX == false) {
            @mail($email,
                  rinse($subj) . txfield('sdesc'),
                  wordwrap($in['assign'] . ' you have just been assigned to this bug by ' . $from . "\n\n") .
                  $dev_text,
                  "From: $from\n".
                  "X-PHP-Bug: $bug[id]\n".
                  "In-Reply-To: <bug-$bug[id]@$site.php.net>",
                  "-fpear-sys@php.net");
        }
    }
}

/**
 * Turns a unix timestamp into a uniformly formatted date
 *
 * If the date is during the current year, the year is omitted.
 *
 * @param int $date  the unix timestamp to be formatted
 *
 * @return string  the formatted date
 */
function format_date($date) {
    return date('Y-m-d H:i', $date - date('Z', $date)) . ' UTC';
}

/**
 * Produces a string containing the bug's prior comments
 *
 * @param int $bug_id  the bug's id number
 * @param int $all     should all existing comments be returned?
 *
 * @return string  the comments
 */
function get_old_comments($bug_id, $all = 0)
{
    global $dbh, $site;
    $divider = str_repeat("-", 72);
    $max_message_length = 10 * 1024;
    $max_comments = 5;
    $output = ""; $count = 0;

    $res =& $dbh->query("SELECT ts, email, comment FROM bugdb_comments WHERE bug=$bug_id ORDER BY ts DESC");

    # skip the most recent unless the caller wanted all comments
    if (!$all) {
        $row =& $res->fetchRow(DB_FETCHMODE_ORDERED);
        if (!$row) {
            return '';
        }
    }

    while (($row =& $res->fetchRow(DB_FETCHMODE_ORDERED)) && strlen($output) < $max_message_length && $count++ < $max_comments) {
        $output .= "[$row[0]] ". spam_protect($row[1], 'text') ."\n\n$row[2]\n\n$divider\n\n";
    }

    if (strlen($output) < $max_message_length && $count < $max_comments) {
        $res =& $dbh->query("SELECT ts1,email,ldesc FROM bugdb WHERE id=$bug_id");
        if (!$res) {
            return $output;
        }
        $row =& $res->fetchRow(DB_FETCHMODE_ORDERED);
        if (!$row) {
            return $output;
        }
        return ("\n\nPrevious Comments:\n$divider\n\n" . $output . "[$row[0]] ". spam_protect($row[1], 'text') ."\n\n$row[2]\n\n$divider\n\n");
    } else {
        return ("\n\nPrevious Comments:\n$divider\n\n" . $output . "The remainder of the comments for this report are too long. To view\nthe rest of the comments, please view the bug report online at\n    http://$site.php.net/bugs/bug.php?id=$bug_id\n");
    }

    return '';
}

/**
 * Converts any URI's found in the string to hyperlinks
 *
 * @param string $text  the text to be examined
 *
 * @return string  the converted string
 */
function addlinks($text)
{
    $text = htmlspecialchars($text);
    $text = preg_replace("/((mailto|http|ftp|nntp|news):.+?)(&gt;|\\s|\\)|\\.\\s|$)/i","<a href=\"\\1\">\\1</a>\\3",$text);
    # what the heck is this for?
    $text = preg_replace("/[.,]?-=-\"/", '"', $text);
    return $text;
}

/**
 * Determine if the given package name is legitimate
 *
 * @param string $type  the name of the package
 *
 * @return bool
 */
function package_exists($type)
{
    global $dbh, $pseudo_pkgs;

    if (empty($type)) {
        return false;
    }
    if (in_array($type, $pseudo_pkgs)) {
        return true;
    }
    $found = $dbh->getOne('SELECT count(*) FROM packages WHERE packages.name=' . "'" . $type ."'");
    return $found == 1 ? true : false;
}

/**
 * Validate an incoming bug report
 *
 * @param
 *
 * @return void
 */
function incoming_details_are_valid($in, $initial = 0)
{
    global $bug, $dbh, $types;

    $errors = array();
    if (empty ($in['package_name']) || $in['package_name'] == 'none') {
        $errors[] = 'Please select an appropriate bug type.';
    } else if (!package_exists($in['package_name'])) {
        $errors[] = 'Please select an appropriate bug type.';
    }

    if ($initial || (!empty($in[email]) && $bug[email] != $in[email])) {
        if (!preg_match("/[.\\w+-]+@[.\\w-]+\\.\\w{2,}/i",$in['email'])) {
            $errors[] = 'Please provide a valid email address.';
        }
    }

    if (isset($in['php_version']) && $in['php_version'] == 'earlier') {
        $errors[] = 'Please select a valid PHP version. If your PHP version is too old, please upgrade first and see if the problem has not already been fixed.';
    }

    if (!array_key_exists($in['bug_type'], $types)) {
        $errors[] = 'Please select a valid bug type.';
    }

    if (empty($in['php_version'])) {
        $errors[] = 'Please select a valid PHP version.';
    }

    if (empty($in['sdesc'])) {
        $errors[] = 'You must supply a short description of the bug you are reporting.';
    }

    if ($initial && empty($in['ldesc'])) {
        $errors[] = 'You must supply a long description of the bug you are reporting.';
    }

    if ($initial && empty($in['passwd'])) {
        $errors[] = 'You must supply a password for this bug report.';
    }

    return $errors;
}

/**
 * Produces an array of eamil addresses the report should go to
 *
 * @param string $package_name  the package's name
 *
 * @return array  an array of eamil addresses
 */
function get_package_mail($package_name)
{
    global $site, $bugEmail;
    // XXX Change to pecl-bugs when it gets opened
    //$to[] = "pecl-dev@lists.php.net";

    switch ($package_name) {
        case 'Bug System':
        case 'PEPr':
        case 'Web Site':
            return array('pear-webmaster@lists.php.net', 'pear-webmaster@lists.php.net');
        case 'Documentation':
            return array('pear-doc@lists.php.net', 'pear-doc@lists.php.net');
    }

    $maintainers = package::info($package_name, 'authors');

    $to = array();
    foreach ($maintainers as $data) {
        $to[] = $data['email'];
    }

    return array(implode(', ', $to), $bugEmail);
}

/**
 * Prepare a query string with the search terms
 *
 * @param string $search  the term to be searched for
 *
 * @return array
 */
function format_search_string($search)
{
    // Function will be updated to make results more relevant.
    // Quick hack for indicating ignored words.

    global $mysql4;

    $min_word_len=3;

    $words = preg_split("/\s+/", $search);
    $ignored = $used = array();
    foreach($words AS $match) {
        if (strlen($match) < $min_word_len) {
            array_push($ignored, $match);
        } else {
            array_push($used, $match);
        }
    }

    if ($mysql4 && BOOLEAN_SEARCH) {
        // require all used words (all)
        if (BOOLEAN_SEARCH === 1) {
            $newsearch = '';
            foreach ($used as $word) {
                $newsearch .= "+$word ";
            }
            return array(" AND MATCH (bugdb.email,sdesc,ldesc) AGAINST ('" . escapeSQL($newsearch) . "' IN BOOLEAN MODE)", $ignored);

        // allow custom boolean search (raw)
        } elseif (BOOLEAN_SEARCH === 2) {
            return array(" AND MATCH (bugdb.email,sdesc,ldesc) AGAINST ('" . escapeSQL($search) . "' IN BOOLEAN MODE)", $ignored);
        }
    }
    // require any of the words (any)
    return array(" AND MATCH (bugdb.email,sdesc,ldesc) AGAINST ('" . escapeSQL($search) . "')", $ignored);
}

/**
 * Print the bug navigation menu
 *
 * The package home page and report new bug links are only displayed
 * if the bug/search being looked at only covers a single package.
 *
 * @param string|array $pkgname  the names of the packages the user is
 *                               presently viewing bugs for
 *
 * @return void
 */
function show_bugs_menu($pkgname, $status = null)
{
    global $dbh, $pseudo_pkgs;

    if (empty($pkgname)) {
        return;
    }

    $pkgname = (array)$pkgname;
    $home    = true;

    foreach ($pkgname as $key => $value) {
        if (!preg_match('/^[A-Za-z0-9_ ]+$/', $value)) {
            return;
        }
        if (in_array($value, $pseudo_pkgs)) {
            $home = false;
            $pkgname[$key] = strtr($value, ' ', '+');
        }
    }
    $pkgs = 'package_name[]=' . implode('&amp;package_name[]=', $pkgname);

    echo "\n\n<!-- BEGIN BUG NAV -->\n\n";
    echo '<table width="100%" border="0"><tr><td>' . "\n ";

    if (count($pkgname) == 1) {
        if ($home) {
            echo make_link('/package/' . $pkgname[0], 'Package home');
            echo delim();
        }
        echo make_link('/bugs/report.php?package=' . $pkgname[0],
                       'Report new bug');
        echo delim();
    }

    echo make_link('/bugs/search.php', 'New search');

    echo "\n</td>\n";

    $style['Open']     = ($status == 'Open')    ?' class="status_active"':'';
    $style['Feedback'] = ($status == 'Feedback')?' class="status_active"':'';
    $style['All']      = ($status == 'All')     ?' class="status_active"':'';

    echo '<td style="text-align: right;">' . "\n Status: ";
    echo make_link('/bugs/search.php?cmd=display&amp;' . $pkgs .
                   '&amp;status=Open', 'Open', '', $style['Open']);
    echo delim();
    echo make_link('/bugs/search.php?cmd=display&amp;' . $pkgs .
                   '&amp;status=Feedback', 'Feedback', '', $style['Feedback']);
    echo delim();
    echo make_link('/bugs/search.php?cmd=display&amp;' . $pkgs .
                   '&amp;status=All', 'All', '', $style['All']);

    echo "\n</td></tr></table>\n";
    echo '<hr />';
    echo "\n\n<!-- END BUG NAV -->\n\n";
}

?>