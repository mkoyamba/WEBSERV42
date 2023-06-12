<html>
<body>
<?php

$filename = $_FILES['file']['name'];

$location = "uploads/".$filename;

if ( move_uploaded_file($_FILES['file']['tmp_name'], $location) ) { 
  echo '<p>The HTML5 and php file upload was a success!</p>'; 
} else { 
  echo '<p>The php and HTML5 file upload failed.</p>'; 
}

?>
</body>
</html>