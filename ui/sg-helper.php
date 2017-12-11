<?php
/**
 * @package sg-helper
 */
/*
Plugin Name: sg-helper
Description: Used to add functionality to smart-greenhouse user interface.
Version: 1.0
Author: dec1711
Author URI: dec1711.sd.ece.iastate.edu
License: GPLv2 or later
Text Domain: sg-helper
*/

// block direct access to plugin
defined( 'ABSPATH' ) or die( 'SMDB!' );

// length of table name taken from chamber title during chamber table creation
define('TABLE_LENGTH_FROM_TITLE', 20);
// prefix for plugin metadata tables
define('PLUGIN_META_TABLES_PREFIX', 'sg_');
// prefix for chamber data tables
define('CHAMBER_DATA_TABLES_PREFIX', 'cd_');
// nonce to verify ajax requests
define('AJAX_NONCE', 'stringy-muff-cabbage');

// add custom js and css to plugin
add_action( 'admin_footer', 'load_chamber_js_css' );
add_action('wp_enqueue_scripts', 'load_chamber_js_css');

function load_chamber_js_css() {
	// register styles
	wp_enqueue_style('sg-helper.css', plugins_url('css/sg-helper.css', __FILE__));
	// register scripts
	wp_register_script('gcharts_loader_js', 'https://www.gstatic.com/charts/loader.js');
	wp_enqueue_script( 'sg-helper.js', plugin_dir_url(__FILE__) . '/js/sg-helper.js',
											array('jquery', 'jquery-ui-dialog', 'gcharts_loader_js'));
  wp_localize_script( 'sg-helper.js', 'ajax_object', array(
		// URL to wp-admin/admin-ajax.php to process the request
		'ajaxurl' => admin_url( 'admin-ajax.php' ),
		// generate a nonce with a unique ID to verify AJAX requests
	  'security' => wp_create_nonce( AJAX_NONCE )
  ));
}

// enable actions for admin and regular users
if(is_admin()) {
	add_action( 'wp_ajax_load_chamber', 'load_chamber' );
	add_action( 'wp_ajax_nopriv_load_chamber', 'load_chamber' );
	add_action( 'wp_ajax_add_chamber', 'add_chamber' );
	add_action( 'wp_ajax_nopriv_load_chamber', 'add_chamber' );
	add_action( 'wp_ajax_deactivate_chamber', 'deactivate_chamber' );
	add_action( 'wp_ajax_nopriv_deactivate_chamber', 'deactivate_chamber' );
	add_action( 'wp_ajax_chamber_table_init', 'chamber_table_init' );
	add_action( 'wp_ajax_nopriv_chamber_table_init', 'chamber_table_init' );
	add_action( 'wp_ajax_sg_load_dashboard', 'sg_load_dashboard' );
	add_action( 'wp_ajax_nopriv_sg_load_dashboard', 'sg_load_dashboard' );
	add_action( 'wp_ajax_update_details', 'update_details' );
	add_action( 'wp_ajax_nopriv_update_details', 'update_details' );
}
// add non-ajax action hooks in else clause if necessary


/* Load initial chamber table information for all chambers */
function chamber_table_init() {
	// check nonce, make sure request is internal
	check_ajax_referer(AJAX_NONCE, 'security');
	// initialize DB access
	global $wpdb;
	// get general chamber info
	// if($_POST['isActive']) {
		$chamber_info = get_active_chamber_info($wpdb);
	// } else {
	// 	$chamber_info = get_history_chamber_info($wpdb);
	// }
	echo stripslashes_deep(json_encode($chamber_info));
	// terminate to return response immediately
	wp_die();
} // end chamber table initialization

/* get all active chamber info and return */
function get_active_chamber_info($wpdb) {
	$table_name = PLUGIN_META_TABLES_PREFIX."allchambers";
	return $wpdb->get_results("SELECT number, active, title, type, interval_length, interval_type, details FROM ".$table_name);
} // end get all chamber info

// /* get all history chamber info and return */
// function get_history_chamber_info($wpdb) {
// 	$table_name = PLUGIN_META_TABLES_PREFIX."chamber_history";
// 	return $wpdb->get_results("SELECT title, type, interval_length, interval_type, details FROM ".$table_name);
// } // end get all chamber info

/* Get specific table information and send back to populate dashbaord */
function sg_load_dashboard() {
	check_ajax_referer(AJAX_NONCE, 'security');
	global $wpdb;
	// get table name for allchambers or history table
	// if($_POST['isActive']) {
		$table_allchambers = PLUGIN_META_TABLES_PREFIX."allchambers";
	// } else {
	// 	$table_allchambers = PLUGIN_META_TABLES_PREFIX."chamber_history";
	// }
	// get table name from table
	$table_name = $wpdb->get_var($wpdb->prepare(
		"SELECT table_name FROM ".$table_allchambers." WHERE number = %d",
		$_POST['number']
	));
	// get sensor data from chamber's table
	$response = $wpdb->get_results("SELECT * FROM ".$table_name);
	echo stripslashes_deep(json_encode($response));
	wp_die();
}

/* update chamber details */
function update_details() {
	check_ajax_referer(AJAX_NONCE, 'security');
	global $wpdb;
	// get table name to udpate
	$allchambers = PLUGIN_META_TABLES_PREFIX."allchambers";
	// update database
	$wpdb->update($allchambers,
		array( 'details' => $_POST['details'] ),
		array( 'number' => $_POST['number'] ),
		array( '%s' ),
		array( '%d' )
	);
	wp_die();
}

/* add new chamber to prepare for data collection */
function add_chamber() {
	// check nonce
	check_ajax_referer(AJAX_NONCE, 'security');
	// initialize db access
	global $wpdb;
	// create unique table name from title
	$table_name = create_table_name($_POST['title'], $wpdb);
	// add table to db (returns sql query results if needed)
  create_chamber_table($table_name);
	// update sg_allchambers with new chamber info
	update_allchambers_activation($table_name, $wpdb);
	// force return
	wp_die();
} // end add chamber

// /* Add custom schedules for sensor read reqeusts */
// function my_cron_schedules($schedules){
//     if(!isset($schedules["5min"])){
//         $schedules["5min"] = array(
//             'interval' => 5*60,
//             'display' => __('Once every 5 minutes'));
//     }
//     if(!isset($schedules["30min"])){
//         $schedules["30min"] = array(
//             'interval' => 30*60,
//             'display' => __('Once every 30 minutes'));
//     }
//     return $schedules;
// }
// add_filter('cron_schedules','my_cron_schedules');
//
// // to schedule event with custom schedule
// wp_schedule_event(time(), '5min', 'my_schedule_hook', $args);
//
// // to add schedule when adding a chamber...
// add_action('my_hourly_event', 'do_this_hourly');
//
// function my_activation() {
// 	wp_schedule_event( current_time( 'timestamp' ), 'hourly', 'my_hourly_event');
// }
//
// function do_this_hourly() {
// 	// do something every hour
// }

// to remove schedule
function my_deactivation() {
	wp_clear_scheduled_hook('my_hourly_event');
}

/* updates sg_allchambers table with new chamber info */
function update_allchambers_activation($table_name, $wpdb) {
	$allchambers = PLUGIN_META_TABLES_PREFIX."allchambers";
	$wpdb->update($allchambers,
		array( 'active' => 1,
					 'table_name' => $table_name,
					 'title' => $_POST['title'],
					 'type' => $_POST['type'],
					 'interval_length' => $_POST['interval_length'],
					 'interval_type' => $_POST['interval_type'],
					 'details' => $_POST['details'] ),
		array( 'number' => $_POST['number'] ),
		array( '%d', '%s', '%s', '%s', '%d', '%s', '%s' ),
		array( '%d' )
	);
} // end update all chambers activation

/* create new table for chamber in db */
function create_chamber_table($table_name) {
	// get charset and collate settings from db
	$collate = 'COLLATE utf8_general_ci';
	// create sql query to execute with dbDelta
	$sql = "CREATE TABLE $table_name (
		datetime datetime NOT NULL,
		o2_ppm float UNSIGNED NOT NULL,
		co2_ppm float UNSIGNED NOT NULL,
		temp_degC float NOT NULL,
		humidity float NOT NULL,
		KEY datetime (datetime)
	)	$collate;";
	// include dbDelta functionality temporarily
	require_once(ABSPATH . 'wp-admin/includes/upgrade.php');
	// execute query and return query results
	return dbDelta( $sql );
} // end create chamber table

/* create a unique table name from the chamber's title */
function create_table_name($title, $wpdb) {
	$table_name = CHAMBER_DATA_TABLES_PREFIX;
	$title = str_replace(' ', '_', $title);
	// copy entire title if less than TABLE_LENGTH_FROM_TITLE
	if(strlen($title) <= TABLE_LENGTH_FROM_TITLE) {
		$table_name .= $title;
	} else {
		$table_name .= substr($title, 0, TABLE_LENGTH_FROM_TITLE);
	}
	$table_dup = PLUGIN_META_TABLES_PREFIX."table_duplicates";
	// if table already exists, increment
	if($wpdb->get_var("SHOW TABLES LIKE '$table_name'") != $table_name) {
		// add entry in duplicates table
		$wpdb->insert($table_dup,
			array('table_name' => $table_name),
			array('%s')
		);
	} else {
		// get number from duplicates table
		$next_int = $wpdb->get_var($wpdb->prepare(
			"SELECT current_number FROM ".$table_dup." WHERE table_name = %s",
			$table_name
		));
		// increment and update duplicates table
		$wpdb->update($table_dup,
			array( 'current_number' => ++$next_int),
			array( 'table_name' => $table_name),
			array( '%d' ),
			array( '%s' )
		);
		$table_name .= ($next_int - 1);
	}
	return $table_name;
} // end create table name


/* deactivate chamber, reset tables and update history */
function deactivate_chamber() {
	// check nonce
	check_ajax_referer(AJAX_NONCE, 'security');
	// initialize db access
	global $wpdb;
	// get chamber info from sg_all_chambers to add to history
	$chamber_info = get_chamber_info($_POST['number'], $wpdb);
	// add chamber info to sg_chamber_history
	add_to_history($chamber_info, $wpdb);
	// update sg_allchambers to reset deactivated chamber info
	update_allchambers_deactivation($wpdb);
	// force return
	wp_die();
} // end add chamber

/* get chamber info for chamber number and return */
function get_chamber_info($chamber_number, $wpdb) {
	$table_name = PLUGIN_META_TABLES_PREFIX."allchambers";
	return $wpdb->get_row($wpdb->prepare(
		"SELECT * FROM ".$table_name." WHERE number = %d",
		$chamber_number),
		ARRAY_A
	);
} // end get chamber info

/* updates sg_allchambers table during deactivation */
function update_allchambers_deactivation($wpdb) {
	$table_name = PLUGIN_META_TABLES_PREFIX."allchambers";
	$wpdb->update($table_name,
		array( 'active' => 0,
					 'table_name' => NULL,
					 'title' => NULL,
					 'type' => NULL,
					 'interval_length' => 0,
					 'interval_type' => NULL,
					 'details' => NULL ),
		array( 'number' => $_POST['number'] ),
		array( '%d', '%s', '%s', '%s', '%d', '%s', '%s' ),
		array( '%d' )
	);
} // end update all chambers deactivation

/* add chamber to sg_chamber_history */
function add_to_history($chamber_info, $wpdb) {
	$table_name = PLUGIN_META_TABLES_PREFIX."chamber_history";
	$wpdb->insert($table_name,
		array( 'table_name' => $chamber_info['table_name'],
	 				 'title' => $chamber_info['title'],
				 	 'type_at_deactivation' => $chamber_info['type'],
					 'interval_length' => $chamber_info['interval_length'],
					 'interval_type' => $chamber_info['interval_type'],
				 	 'details' => $chamber_info['details'] ),
		array( '%s', '%s', '%s', '%d', '%s', '%s' )
	);
} // end add to history
