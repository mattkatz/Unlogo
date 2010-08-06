<?php
function error($code, $message)
{
	$error = array("code"=>$code, "message"=>$message);
	$errors = array("status"=>"error", "errors"=>array($error));	
	print json_encode($errors);	
}

if (strpos($_SERVER['HTTP_USER_AGENT'],"iPhone") == true || isset($_REQUEST['debug']))
{
	require_once("db.php");
	function __autoload($class_name) {
		require_once $class_name . '.class.php';
	}
	
	// Log some stuff
	$log = new KLogger ( "log.txt" , KLogger::DEBUG );
	$log->LogInfo("\n\n\n=================\n");
	$log->LogInfo("REQUEST VARS: ".var_export($_REQUEST, true));
	$log->LogInfo("FILES: ".var_export($_FILES, true)."\n\n\n");
	
	// We need to have an action to do anything.
	if(!isset($_REQUEST['action'])) {
		error("NO_ACTION", "No action provided");
		$log->LogInfo("User did not provide an action.  Quitting.");
		exit;
	}
	
	// Try to construct the user.
	if(!isset($_REQUEST['udid'])) {
		error("NO_UDID", "No udid provided");	
		$log->LogInfo("User did not provide a UDID.  Quitting.");
		exit;
	}
	
	// Create the user first.
	$user = new User();
	
	// Create the user if they are not already 
	$user->loadFromUDID($_REQUEST['udid']);
	
	
	if(isset($_FILES['device_token'])) {
		if(!$user->readDeviceTokenFromFile($_FILES['device_token']['tmp_name'])) {
			error("UNREADABLE_TOKEN", "Couldn't read token file.");
			exit;
		}
	}
	if(isset($_REQUEST['device_name'])) {
		$user->device_name = utf8_encode($_REQUEST['device_name']);
	}

	if(!$user->save()) {
		$errors = array("status"=>"error", "errors"=>$user->errors);	
		print json_encode($errors);
		exit;
	}
	
	// This array will be json_encoded and will be the only output of this fil.
	$return = array();
	
	// What are we doing here?
	switch($_REQUEST['action']) {
		case 'upload':
			$media = new MediaFile();
			$media->user_id = $user->id;
			if( $media->doUpload("file", "../uploads/") && $media->save() && $media->process() ) {
				$return["status"] = "ok";
			} else {
				$return['status'] = "error";
				$return["errors"] = $media->errors;
			}
			break;
		case 'sync':
			$media = json_decode( $_REQUEST['media'], true );
			$log->LogInfo( "media: ".var_export($media, true) );
		
			$return["status"] = "ok";
			$return["media"] = $user->getMedia();
			break;
		default:
			$return["status"] = "error";
			$return["errors"] = array("Command '{$_REQUEST['action']}' not recognized.");
			break;
	}
	
	print json_encode( $return );
	exit;
}
else
{
	include("goaway.html");
	print_r($_SERVER);
}
?>