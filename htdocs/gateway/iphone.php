<?php
if (strpos($_SERVER['HTTP_USER_AGENT'],"iPhone") == true || isset($_REQUEST['debug']))
{
	require_once("db.php");
	function __autoload($class_name) {
		require_once $class_name . '.class.php';
	}
	
	$log = new KLogger ( "log.txt" , KLogger::DEBUG );
	$log->LogInfo("REQUEST VARS: ".var_export($_REQUEST, true));
	$log->LogInfo("FILES: ".var_export($_FILES, true));

	// We need to have an action to do anything.
	if(!isset($_REQUEST['action'])) {
		$errors = array("status"=>"error", "errors"=>array("No action provided"));	
		print json_encode($errors);
		$log->LogInfo("User did not provide an action.  Quitting.");
		exit;
	}
	
	// Try to construct the user.
	if(!isset($_REQUEST['udid'])) {
		$errors = array("status"=>"error", "errors"=>array("No udid provided"));	
		print json_encode($errors);
		$log->LogInfo("User did not provide a UDID.  Quitting.");
		exit;
	}
	
	// Create the user first.
	$user = new User();
	
	// Create the user if they are not already 
	$user->loadFromUDID($_REQUEST['udid']);
	
	
	if(isset($_FILES['device_token'])) {
		$ok = $user->readDeviceTokenFromFile($_FILES['device_token']['tmp_name']);
		if(!$ok) {
			$errors = array("status"=>"error", "errors"=>array("Couldn't read token file."));	
			print json_encode($errors);
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
			$ok = $media->doUpload("file", "../uploads/");
			if($ok) {
				$return["status"] = "ok";
				$media->save();
				$media->process();
			} else {
				$return['status'] = "error";
				$return["errors"] = $media->errors;	
			}
			break;
		case 'sync':
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
}
?>