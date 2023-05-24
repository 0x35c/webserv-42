var xMax = document.body.clientWidth - 200;
var yMax = document.body.clientHeight - 200;

var x = Math.random() * xMax;
var y = Math.random() * yMax;
var xSpeed = 2 * (Math.random() < 0.5 ? -1 : 1);
var ySpeed = 2 * (Math.random() < 0.5 ? -1 : 1);

var audio1 = new Audio('https://www.myinstants.com/media/sounds/aughhhhh-aughhhhh.mp3');
var audio2 = new Audio('https://www.myinstants.com/media/sounds/among-us-role-reveal-sound.mp3');

var stop = false;

document.body.style.backgroundImage = 'url("chriscelaya.jpg")';

addEventListener('resize', function() {
	xMax = document.body.clientWidth - 200;
	yMax = document.body.clientHeight - 200;
	if (x > xMax) {
		x = xMax;
	}
	if (y > yMax) {
		y = yMax;
	}
	document.body.style.backgroundPositionX = x + 'px';
	document.body.style.backgroundPositionY = y + window.scrollY + 'px';
});

addEventListener('scroll', function() {
	yMax = document.body.clientHeight - 200;
	if (y > yMax) {
		y = yMax;
	}
	this.document.body.style.backgroundPositionY = y + window.scrollY + 'px';
});

addEventListener('click', function(event) {
	if (stop) {
		return;
	}

	if (event.clientX > x && event.clientX < x + 200 && event.clientY > y && event.clientY < y + 200) {
		if (document.body.style.backgroundImage == 'url("chriscelaya.jpg")') {
			audio1.currentTime = 0.5;
			audio1.play();
			document.body.style.animation = 'shake 0.5s linear infinite';
			stop = true;
		}
		else if (document.body.style.backgroundImage == 'url("averdon.png")') {
			audio2.currentTime = 0.5;
			audio2.play();
			document.body.style.transition = 'all 0.5s';
			document.body.style.backgroundPosition = 'center';
			document.body.style.backgroundSize = 'cover';
			stop = true;
		}
	}
});

audio1.addEventListener('ended', function() {
	document.body.style.animation = '';
	stop = false;
});

audio2.addEventListener('ended', function() {
	document.body.style.transition = '';
	document.body.style.backgroundSize = 'auto';
	stop = false;
});

function move() {
	if (stop) {
		return;
	}

	x += xSpeed;
	y += ySpeed;
	if (x > xMax) {
		x = xMax;
		xSpeed = -xSpeed;
		document.body.style.backgroundImage = 'url("averdon.png")';
	}
	if (x < 0) {
		x = 0;
		xSpeed = -xSpeed;
		document.body.style.backgroundImage = 'url("tguerin.png")';
	}
	if (y > yMax) {
		y = yMax;
		ySpeed = -ySpeed;
		document.body.style.backgroundImage = 'url("ulayus.png")';
	}
	if (y < 0) {
		y = 0;
		ySpeed = -ySpeed;
		document.body.style.backgroundImage = 'url("chriscelaya.jpg")';
	}
	document.body.style.backgroundPositionX = x + 'px';
	document.body.style.backgroundPositionY = y + window.scrollY + 'px';
}

setInterval(move, 10);
