<?php

class MediaFile {

	var $original;  // The local (server) filename for the media

	var $media_id;  // The system-wide unique identifier for the media

	var $errors = array();

	var $media_type = NULL; // $valid_media_types = array("image", "video");

	var $user_id;

	var $status; // $valid_statuses = array("new", "queued", "processing", "done");
	
	var $in_database = false;
	
	var $title="";
	
	var $download_link;
	
	function MediaFile($id=NULL) {
		if($id!=NULL) {
			$this->id = intval($id);
			$this->in_database = $this->load();	
		} 
	}
	
	function load() {
		if(!isset($this->id)) {
			return false;
		}
		
		$sql="SELECT original, download_link, title, media_id, media_type, user_id, status, date_modified, date_uploaded 
			FROM media WHERE id={$this->id} LIMIT 1";
		$result = mysql_query($sql);
		if(mysql_errno() > 0) {
			$this->errors[] = array("code"=>"NO_MEDIA_ID", "message"=>"Couldn't load media id={$this->id}");
			return false;
		}
		$row = mysql_fetch_assoc($result);
		$this->title = $row['title'];
		$this->download_link = $row['download_link'];
		$this->media_id = $row['media_id'];
		$this->original = $row['original'];
		$this->media_type = $row['media_type'];
		$this->user_id = $row['user_id'];
		$this->status = $row['status'];
		$this->date_uploaded = $row['date_uploaded'];
		return true;
	}
	
	
	function process() {
		// This is where the unlogo exe will be called. 
		// For now, just make a copy
		if(!copy("../uploads/{$this->original}", "../processed/{$this->original}")) {
			$this->errors[] = array("code"=>"PROCESSING_ERROR", "message"=>"There was an error processing the file.");
			return false;	
		}
		$this->status = 'done';
		$this->download_link = "http://{$_SERVER['SERVER_NAME']}/processed/{$this->original}";
		if(!$this->save()) {
			return false;	
		}
		return true;
	}
	
	function getOwner() {
		if(isset( $this->user_id )) {
			return new User( $this->user_id );
		} else {
			return false;
		}
	}
	
	/*
	function generateMediaID() {
		$arrIp = explode('.', $_SERVER['REMOTE_ADDR']);
		list($usec, $sec) = explode(' ', microtime());
		$usec = (integer) ($usec * 65536);
		$sec = ((integer) $sec) & 0xFFFF;
		$strUid = sprintf("%08x-%04x-%04x", ($arrIp[0] << 24) | ($arrIp[1] << 16) | ($arrIp[2] << 8) | $arrIp[3], $sec, $usec);
		return $strUid;
	}
	*/
	
	function doUpload($files_index="file", $destination="uploads/") {
		global $_FILES, $_SERVER, $_REQUEST;
	
		if($destination[strlen($destination)-1] != "/")
			$destination .= "/";
			
		if(!isset($_FILES[$files_index])) {
			$this->errors[] = array("code"=>"NO_FILE_POSTED", "message"=>"No file was posted.");
			return false;
		}
	
		// Ty to open the upload directory.
		if (!($handle = opendir($destination))) {
			$this->errors[] = array("code"=>"UPLOAD_DIR", "message"=>"couldn't open upload directory.");
			return false;
		}
		
		if(empty($_REQUEST['media_id'])) {
			$this->errors[] = array("code"=>"NO_MEDIA_ID", "message"=>"No media ID was provided.");
			return false;
		}
		
		$this->title = $_REQUEST['title'];
		$this->media_id = $_REQUEST['media_id'];
		$sql="SELECT id FROM media WHERE media_id='{$this->media_id}' ";
		$result = mysql_query($sql);
		if(mysql_num_rows($result)>0) {
			$this->errors[] = array("code"=>"MEDIA_ID_EXISTS", "message"=>"The media ID {$this->media_id} already exists.");
			return false;
		}
	
		if(eregi('image', $_FILES[$files_index]['type'])) {
			
			if(eregi("jpeg", $_FILES[$files_index]['type'])) {
				$ext = ".jpg";
			} else if(eregi("png", $_FILES[$files_index]['type'])) {
				$ext = ".png";
			} else {
				$this->errors[] = array("code"=>"UNKNOWN_IMAGE_FORMAT", "message"=>"Image not recognized.  Please upload a jpeg or png image.");
				return false;
			}
			
			$this->original = $this->media_id.$ext;
			$this->media_type = "image";
		} else if(eregi('video', $_FILES[$files_index]['type'])) {
			$this->media_type = "video";
			$this->original = $this->media_id.".mov";
		} else {
			$this->errors[] = array("code"=>"UNKNOWN_UPLOAD_FORMAT", "message"=>"The uploaded file is not an image or video. Please upload a valid file!");
			return false;
		}
		
		$new_path = $destination.$this->original;
		if(file_exists($new_path)) {
			$this->errors[] = array("code"=>"UPLOAD_FILE_EXISTS", "message"=>"This file has already been uploaded.");
			return false;	
		}
		
		if (!move_uploaded_file($_FILES[$files_index]['tmp_name'], $new_path)) {
			$this->errors[] = array("code"=>"MOVE_UPLOAD_FAILED", "message"=>"couldn't move uploaded file. possible file upload attack!");
			return false;
		}
	
		return true;
	}
	
	function save() {
		global $_FILES, $_REQUEST;
	
		$file = basename($this->original);
		$file = mysql_escape_string($file);
		$title = mysql_escape_string($this->title);
		$link = mysql_escape_string($this->download_link);
		
		if($this->in_database) {
			$sql="UPDATE media SET 
				original = '$file',
				title = '$title',
				media_id = '{$this->media_id}',
				media_type='{$this->media_type}',
				download_link='$link',
				user_id = $this->user_id,
				status = '$this->status'
				WHERE id={$this->id} LIMIT 1";
		} else {
			$sql="INSERT INTO media SET 
				original='$file', 
				title = '$title',
				media_id = '{$this->media_id}',
				media_type='{$this->media_type}',
				date_uploaded=NOW(), 
				user_id = $this->user_id,
				status='new'";
			$this->in_database = true;
		}
		
		mysql_query($sql);
		if(mysql_errno()>0) {
			$this->errors[] = array("code"=>"SQL_ERROR", "message"=>mysql_error());
			return false;
		} else {
			$this->id = mysql_insert_id();
			return true;
		}
	}
}	
?>