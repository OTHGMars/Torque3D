    var slider = document.getElementById("ValSlider");
    var output = document.getElementById("SliderOutput");
    output.innerHTML = slider.value;

    slider.oninput = function() {
        output.innerHTML = this.value;
    }

    function DemoButtonClicked() 
	{
        var txtCtrl = document.getElementById("UserTextBox");
        var numCtrl = document.getElementById("FloatTestBox");
        var checkCtrl = document.getElementById("BoolTestBox");
        var selectCtrl = document.getElementById("Peeps");
		var textArg = txtCtrl.value;
		var iArg = slider.value;
		var fArg = numCtrl.value;
		var bArg = checkCtrl.checked ? true : false;

		if (window.execTS)
		    window.execTS("DemoBtnClick", textArg, iArg, fArg, bArg, selectCtrl.value);
    }

    function SetMainButtonText(text)
	{
        var txtCtrl = document.getElementById("MainDemoButton");
		txtCtrl.innerHTML = text;
    }

    function HideVideoCtrl()
	{
        var vidCtrl = document.getElementById("video1");
        vidCtrl.style.display = 'none';
    }

    function StartVideo(level, vidName)
    {
        var vidCtrl = document.getElementById("video1");
        vidCtrl.style.display = '';

        vidCtrl.src = vidName;
        vidCtrl.volume = level;

        vidCtrl.play();
        vidCtrl.playbackRate = 1.0;
    }
