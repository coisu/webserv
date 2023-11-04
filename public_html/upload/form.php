<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Example of a CGI request</title>
</head>
<body>
    <h1>CGI request form</h1>

    <div class="container">
		<div class="form">
		<?php
			echo "<h1> CGI for ". $_SERVER['REQUEST_METHOD']."  request</h1>";
			echo "<hr>";
			if ($_SERVER['REQUEST_METHOD'] === 'POST')
			{
				$requestBody = file_get_contents('php://input');
				parse_str($requestBody, $_POST);
				$name = $_POST['name'];
				$email = $_POST['email'];
				$message = $_POST['message'];

				echo "<h2>Name:" . $name . "\r\n</h2>";
				echo "<h2>Email:" . $email . "\r\n</h2>";
				echo "<h2>Message:" . $message . "\r\n</h2>";
			}

			if ($_SERVER['REQUEST_METHOD'] === 'GET')
			{
				// $requestBody = file_get_contents('php://input');
				// parse_str($requestBody, $_GET);
				$name = $_GET['name'];
				$email = $_GET['email'];
				$message = $_GET['message'];

				echo "<h2>Name: " . $name . "</h2>";
				echo "<h2>Email: " . $email . "</h2>";
				echo "<h2>Message: " . $message . "</h2>";

			}
		?>
		</div>

    </div>
	<div class="container">
		<button onclick="window.location.href = '/';">Home</button>
	</div>
</body>
</html>