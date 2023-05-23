var xMax = document.body.clientWidth - 200;
var yMax = document.body.clientHeight - 200;

var x = Math.random() * xMax;
var y = Math.random() * yMax;
var xSpeed = 2 * (Math.random() < 0.5 ? -1 : 1);
var ySpeed = 2 * (Math.random() < 0.5 ? -1 : 1);

addEventListener("resize", function() {
	xMax = document.body.clientWidth - 200;
	yMax = document.body.clientHeight - 200;
	if (x > xMax) {
		x = xMax;
	}
	if (y > yMax) {
		y = yMax;
	}
});

function move() {
	x += xSpeed;
	y += ySpeed;
	if (x > xMax) {
		x = xMax;
		xSpeed = -xSpeed;
		document.body.style.backgroundImage = "url('averdon.png')"; // Pas ouf celle la
	}
	if (x < 0) {
		x = 0;
		xSpeed = -xSpeed;
		document.body.style.backgroundImage = "url('tguerin.png')";
	}
	if (y > yMax) {
		y = yMax;
		ySpeed = -ySpeed;
		document.body.style.backgroundImage = "url('ulayus.png')";
	}
	if (y < 0) {
		y = 0;
		ySpeed = -ySpeed;
		document.body.style.backgroundImage = "url('chriscelaya.jpg')";
	}
	document.body.style.backgroundPositionX = x + "px";
	document.body.style.backgroundPositionY = y + "px";
}

setInterval(move, 10);
