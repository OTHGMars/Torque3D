/* ********************************************************************
 **********************************************************************
 * HTML Virtual Keyboard Interface Script - v1.49
 *   Copyright (c) 2011 - GreyWyvern
 *
 *  - Licenced for free distribution under the BSDL
 *          http://www.opensource.org/licenses/bsd-license.php
 *
 * Add a script-driven keyboard interface to text fields, password
 * fields and textareas.
 *
 * See http://www.greywyvern.com/code/javascript/keyboard for examples
 * and usage instructions.
 *
 * Version 1.49 - November 8, 2011
 *   - Don't display language drop-down if only one keyboard available
 *
 *   See full changelog at:
 *     http://www.greywyvern.com/code/javascript/keyboard.changelog.txt
 *
 * Keyboard Credits
 *   - Yiddish (Yidish Lebt) keyboard layout by Simche Taub (jidysz.net)
 *   - Urdu Phonetic keyboard layout by Khalid Malik
 *   - Yiddish keyboard layout by Helmut Wollmersdorfer
 *   - Khmer keyboard layout by Sovann Heng (km-kh.com)
 *   - Dari keyboard layout by Saif Fazel
 *   - Kurdish keyboard layout by Ara Qadir
 *   - Assamese keyboard layout by Kanchan Gogoi
 *   - Bulgarian BDS keyboard layout by Milen Georgiev
 *   - Basic Japanese Hiragana/Katakana keyboard layout by Damjan
 *   - Ukrainian keyboard layout by Dmitry Nikitin
 *   - Macedonian keyboard layout by Damjan Dimitrioski
 *   - Pashto keyboard layout by Ahmad Wali Achakzai (qamosona.com)
 *   - Armenian Eastern and Western keyboard layouts by Hayastan Project (www.hayastan.co.uk)
 *   - Pinyin keyboard layout from a collaboration with Lou Winklemann
 *   - Kazakh keyboard layout by Alex Madyankin
 *   - Danish keyboard layout by Verner Kjærsgaard
 *   - Slovak keyboard layout by Daniel Lara (www.learningslovak.com)
 *   - Belarusian and Serbian Cyrillic keyboard layouts by Evgeniy Titov
 *   - Bulgarian Phonetic keyboard layout by Samuil Gospodinov
 *   - Swedish keyboard layout by Håkan Sandberg
 *   - Romanian keyboard layout by Aurel
 *   - Farsi (Persian) keyboard layout by Kaveh Bakhtiyari (www.bakhtiyari.com)
 *   - Burmese keyboard layout by Cetanapa
 *   - Bosnian/Croatian/Serbian Latin/Slovenian keyboard layout by Miran Zeljko
 *   - Hungarian keyboard layout by Antal Sall 'Hiromacu'
 *   - Arabic keyboard layout by Srinivas Reddy
 *   - Italian and Spanish (Spain) keyboard layouts by dictionarist.com
 *   - Lithuanian and Russian keyboard layouts by Ramunas
 *   - German keyboard layout by QuHno
 *   - French keyboard layout by Hidden Evil
 *   - Polish Programmers layout by moose
 *   - Turkish keyboard layouts by offcu
 *   - Dutch and US Int'l keyboard layouts by jerone
 *
 */
var VNPI_attach, VNPI_close;
(function() {
  var self = this;

  this.VNPI_version = "1.0";
  this.VNPI_showVersion = false;
  this.VNPI_target = false;
  this.VNPI_size = 3;  // Default keyboard size (1-5)
  this.VNPI_activeTab = 0;  // Tab moves to next: 1 = element, 2 = keyboard enabled element
  this.VNPI_keyCenter = 3;
  this.VNPI_hoverRow = 1;
  this.VNPI_hoverCell = 1;

  this.VNPI_isIE = /*@cc_on!@*/false;
  this.VNPI_isIE6 = /*@if(@_jscript_version == 5.6)!@end@*/false;
  this.VNPI_isIElt8 = /*@if(@_jscript_version < 5.8)!@end@*/false;
  this.VNPI_isWebKit = RegExp("KHTML").test(navigator.userAgent);
  this.VNPI_isOpera = RegExp("Opera").test(navigator.userAgent);
  this.VNPI_isMoz = (!this.VNPI_isWebKit && navigator.product == "Gecko");

  /* ***** Create keyboards ************************************** */
  this.VNPI_layout = {};

  /* ***** Layout Number Pad ************************************* */
  this.VNPI_numpad = [
    [["7"], ["8"], ["9"], ["-"]],
    [["4"], ["5"], ["6"], ["CE"]],
    [["1"], ["2"], ["3"], ["\u2190"]],
    [["0"], ["."]]
  ];


  /* ****************************************************************
   * Attach the keyboard to an element
   *
   */
  VNPI_attach = function(elem) {
    if (elem.getAttribute("VNPI_attached"))
      return false;
    elem.onfocus = function() {
      if (self.VNPI_target != this) {
        if (self.VNPI_target)
          self.VNPI_close();
        if (window.controllerNavigation)
          self.VNPI_show(this);
      }
    }
    elem.onclick = function() {
      if (!self.VNPI_target && window.controllerNavigation)
        self.VNPI_show(this);
    }
    elem.setAttribute("VNPI_attached", 'true');
    VNPI_addListener(elem, 'keydown', function(e) {
      if (self.VNPI_target == this) {
        e = e || event;
        return self.VNPI_keynav(e);
      } return false;
    }, false);
    VNPI_addListener(elem, 'keyup', function(e) {
      if (self.VNPI_target == this) {
        e = e || event;
        return self.VNPI_keyUpNav(e);
      } return false;
    }, false);
    VNPI_addListener(elem, 'click', function(e) {
      if (self.VNPI_target == this) {
        e = e || event;
        if (e.stopPropagation) { e.stopPropagation(); } else e.cancelBubble = true;
      } return false;
    }, false);
    if (self.VNPI_isMoz)
      elem.addEventListener('blur', function() { this.setAttribute('_scrollTop', this.scrollTop); }, false);
  };


  /* ***** Find tagged input & textarea elements ***************** */
  function VNPI_buildKeyboardInputs() {
    var inputElems = [
      document.getElementsByTagName('input'),
      document.getElementsByTagName('textarea')
    ];
    for (var x = 0, elem; elem = inputElems[x++];)
      for (var y = 0, ex; ex = elem[y++];)
        if ((ex.type == "number") || (ex.className.indexOf("numpadInput") > -1))
          VNPI_attach(ex);

    VNPI_addListener(document.documentElement, 'click', function(e) { self.VNPI_close(); }, false);
  }


  /* ****************************************************************
   * Common mouse event actions
   *
   */
  function VNPI_mouseEvents(elem) {
    if (elem.nodeName == "TD") {
      if (!elem.click) elem.click = function() {
        var evt = this.ownerDocument.createEvent('MouseEvents');
        evt.initMouseEvent('click', true, true, this.ownerDocument.defaultView, 1, 0, 0, 0, 0, false, false, false, false, 0, null);
        this.dispatchEvent(evt);
      };
      elem.VNPI_clickless = 0;
      VNPI_addListener(elem, 'dblclick', function() { return false; }, false);
    }
  }


  /* ***** Build the numpad interface ************************** */
  this.VNPI_keyboard = document.createElement('table');
  this.VNPI_keyboard.id = "keyboardInputMaster";
  //this.VNPI_keyboard.id = "numpadInputMaster";
  this.VNPI_keyboard.dir = "ltr";
  this.VNPI_keyboard.cellSpacing = "4";
  this.VNPI_keyboard.reflow = function() {
    this.style.width = "50px";
    var foo = this.offsetWidth;
    this.style.width = "";
  };

  VNPI_addListener(this.VNPI_keyboard, 'click', function(e) {
    e = e || event;
    if (e.stopPropagation) { e.stopPropagation(); } else e.cancelBubble = true;
    return false;
  }, false);
  VNPI_addListener(this.VNPI_keyboard, 'keydown', function(e) {
    if (e.keyCode == 37) { //move left or wrap
      active = (active>0)?active-1:active;
    }
    if (e.keyCode == 38) { // move up
        active = (active-columns>=0)?active-columns:active;
    }
    if (e.keyCode == 39) { // move right or wrap
      active = (active<(columns*rows)-1)?active+1:active;
    }
    if (e.keyCode == 40) { // move down
        active = (active+columns<=(rows*columns)-1)?active+columns:active;
    }
    return false;
  }, false);

  //this.VNPI_keyboard.appendChild(thead);

  var tbody = document.createElement('tbody');
    var previewrow = document.createElement('tr');
      var previewdata = document.createElement('td');
        previewdata.className = "previewText";
      previewrow.appendChild(previewdata);
    tbody.appendChild(previewrow);
    
    var tr = document.createElement('tr');
      var td = document.createElement('td');
        var div = document.createElement('div');

      var kbNumpad = document.createElement('td');
          kbNumpad.id = "keyboardInputNumpad";
        var ntable = document.createElement('table');
            ntable.cellSpacing = "1";
          var ntbody = document.createElement('tbody');
            for (var x = 0; x < this.VNPI_numpad.length; x++) {
              var ntr = document.createElement('tr');
              for (var y = 0; y < this.VNPI_numpad[x].length; y++) {
                var ntd = document.createElement('td');
                VNPI_addListener(ntd, 'click', VNPI_keyClick, false);
                VNPI_mouseEvents(ntd);
                ntd.appendChild(document.createTextNode(this.VNPI_numpad[x][y]));
                ntr.appendChild(ntd);
                if (this.VNPI_numpad[x][y] == "0")
                  ntd.colSpan = "3";
              }
              ntbody.appendChild(ntr);
            }
            ntable.appendChild(ntbody);
          kbNumpad.appendChild(ntable);
        tr.appendChild(kbNumpad);
      tbody.appendChild(tr);
  this.VNPI_keyboard.appendChild(tbody);


  /* ****************************************************************
   * Private table cell attachment function for generic characters
   *
   */
  function VNPI_keyClick() {
    var done = false, character = "\xa0";
    if (this.firstChild.nodeName.toLowerCase() != "small") {
      if ((character = this.firstChild.nodeValue) == "\xa0") return false;
    } else
      character = this.firstChild.getAttribute('char');
    self.VNPI_insert(character);
    return false;
  }


  VNPI_addListener(this.VNPI_keyboard, 'selectstart', function() { return false; }, false);
  this.VNPI_keyboard.unselectable = "on";
  if (this.VNPI_isOpera)
    VNPI_addListener(this.VNPI_keyboard, 'mousedown', function() { return false; }, false);


  /* ****************************************************************
   * Insert text at the cursor
   *
   */
  this.VNPI_insert = function(text) {
    this.VNPI_target.focus();
    var tds = this.VNPI_keyboard.tBodies[0].getElementsByTagName('tr')[1];
    var keyrow = tds.getElementsByTagName('tr')[0].getElementsByTagName('td');

    if (text === "-") {
      this.VNPI_numpadText = "-" + this.VNPI_numpadText;
      keyrow[3].innerHTML = "+";
    } else if (text === "+") {
      this.VNPI_numpadText = this.VNPI_numpadText.substr(1);
      keyrow[3].innerHTML = "-";
    } else if (text === "\u2190") {
      var strlen = this.VNPI_numpadText.length - 1;
      this.VNPI_numpadText = this.VNPI_numpadText.substr(0, strlen);
    } else if (text === "CE") {
      this.VNPI_numpadText = "";
    } else if (text === ".") {
      if (this.VNPI_numpadText.search(/[.]/g) == -1) {
        if (this.VNPI_numpadText === "-")
          this.VNPI_numpadText = "-0.";
        else if (this.VNPI_numpadText == "")
          this.VNPI_numpadText = "0."
        else
          this.VNPI_numpadText = this.VNPI_numpadText + text;
      }
    }else {
      this.VNPI_numpadText = this.VNPI_numpadText + text;
    }

    var previewRow = this.VNPI_keyboard.tBodies[0].getElementsByTagName('tr')[0];
    tds = previewRow.getElementsByTagName('td');
    tds[0].innerHTML = this.VNPI_numpadText;
    if (this.VNPI_numpadText === "") {
      tds[0].innerHTML = "0";
      keyrow[3].innerHTML = "-";
    }
};


  /* ****************************************************************
   * Show the keyboard interface
   *
   */
  this.VNPI_show = function(elem) {
    if (!this.VNPI_target) {
      this.VNPI_target = elem;
      try { this.VNPI_keyboard.parentNode.removeChild(this.VNPI_keyboard); } catch (e) {}

      var elem = this.VNPI_target;
      this.VNPI_target.keyboardPosition = "absolute";
      do {
        if (VNPI_getStyle(elem, "position") == "fixed") {
          this.VNPI_target.keyboardPosition = "fixed";
          break;
        }
      } while (elem = elem.offsetParent);

      this.VNPI_kbsize();
      this.VNPI_highlightKey(true, false);

      // Put the current field value into the preview box
      var previewRow = this.VNPI_keyboard.tBodies[0].getElementsByTagName('tr')[0];
      var tds = previewRow.getElementsByTagName('td');
      tds[0].innerHTML = this.VNPI_target.value;
      this.VNPI_numpadText = this.VNPI_target.value;

      // Set the +/- key to the correct state
      tds = this.VNPI_keyboard.tBodies[0].getElementsByTagName('tr')[1];
      var keyrows = tds.getElementsByTagName('tr')[0].getElementsByTagName('td');
      if (this.VNPI_target.value < 0) {
        keyrows[3].innerHTML = "+";
      } else {
        keyrows[3].innerHTML = "-";
      }

      document.body.appendChild(this.VNPI_keyboard);
      this.VNPI_keyboard.style.position = this.VNPI_target.keyboardPosition;

      this.VNPI_position(true);
      if (self.VNPI_isMoz || self.VNPI_isWebKit)
        this.VNPI_position(true);
      this.VNPI_target.blur();
      this.VNPI_target.focus();
    } else
      this.VNPI_close();
  };


  /* ****************************************************************
   * Position the keyboard
   *
   */
  this.VNPI_position = function(force) {
    if (self.VNPI_target) {
      var kPos = VNPI_findPos(self.VNPI_keyboard), wDim = VNPI_innerDimensions(), sDis = VNPI_scrollDist();
      //var place = false, fudge = self.VNPI_target.offsetHeight + 3;
      var place = false, fudge = 2;
      if (force !== true) {
        if (kPos[1] + self.VNPI_keyboard.offsetHeight - sDis[1] - wDim[1] > 0) {
          place = true;
          fudge = -self.VNPI_keyboard.offsetHeight - 3;
        } else if (kPos[1] - sDis[1] < 0) place = true;
      }
      if (place || force === true) {
        var iPos = VNPI_findPos(self.VNPI_target), scr = self.VNPI_target;
        while (scr = scr.parentNode) {
          if (scr == document.body) break;
          if (scr.scrollHeight > scr.offsetHeight || scr.scrollWidth > scr.offsetWidth) {
            if (!scr.getAttribute("VNPI_scrollListener")) {
              scr.setAttribute("VNPI_scrollListener", true);
              VNPI_addListener(scr, 'scroll', function() { self.VNPI_position(true); }, false);
            } // Check if the input is in view
            var pPos = VNPI_findPos(scr), oTop = iPos[1] - pPos[1], oLeft = iPos[0] - pPos[0];
            var top = oTop + self.VNPI_target.offsetHeight;
            var left = oLeft + self.VNPI_target.offsetWidth;
            var bottom = scr.offsetHeight - oTop - self.VNPI_target.offsetHeight;
            var right = scr.offsetWidth - oLeft - self.VNPI_target.offsetWidth;
            self.VNPI_keyboard.style.display = (top < 0 || left < 0 || bottom < 0 || right < 0) ? "none" : "";
            if (self.VNPI_isIE6) self.VNPI_iframe.style.display = (top < 0 || left < 0 || bottom < 0 || right < 0) ? "none" : "";
          }
        }
        self.VNPI_keyboard.style.top = iPos[1] - ((self.VNPI_target.keyboardPosition == "fixed" && !self.VNPI_isIE && !self.VNPI_isMoz) ? sDis[1] : 0) + fudge + "px";
        self.VNPI_keyboard.style.left = Math.max(10, Math.min(wDim[0] - self.VNPI_keyboard.offsetWidth - 25, iPos[0])) + "px";
        if (self.VNPI_isIE6) {
          self.VNPI_iframe.style.width = self.VNPI_keyboard.offsetWidth + "px";
          self.VNPI_iframe.style.height = self.VNPI_keyboard.offsetHeight + "px";
          self.VNPI_iframe.style.top = self.VNPI_keyboard.style.top;
          self.VNPI_iframe.style.left = self.VNPI_keyboard.style.left;
        }
      }
      if (force === true) self.VNPI_position();
    }
  };


  /* ****************************************************************
   * Close the keyboard interface
   *
   */
  this.VNPI_close = VNPI_close = function() {
    if (this.VNPI_target) {
      if ((this.VNPI_numpadText === "") || (this.VNPI_numpadText === "-") ||
        (this.VNPI_numpadText === "-0.") || (this.VNPI_numpadText === "0.")) {
        this.VNPI_target.value = 0;
      }
      else
        this.VNPI_target.value = this.VNPI_numpadText;
      try {
        this.VNPI_keyboard.parentNode.removeChild(this.VNPI_keyboard);
        if (this.VNPI_isIE6) this.VNPI_iframe.parentNode.removeChild(this.VNPI_iframe);
      } catch (e) {}
      this.VNPI_target.focus();
      if (this.VNPI_isIE) {
        setTimeout(function() { self.VNPI_target = false; }, 0);
      } else this.VNPI_target = false;
    }
  };

  /* ****************************************************************
   * Resize the keyboard
   *
   */

  this.VNPI_kbsize = function() {
    if (!window.virtualKeySize)
      window.virtualKeySize = 2;
    self.VNPI_size = Math.min(5, Math.max(1, window.virtualKeySize));
    self.VNPI_keyboard.className = "keyboardInputSize" + self.VNPI_size;
    self.VNPI_position(true);
  };

  /* ****************************************************************
   * Keyboard navigation and highlighting
   *
   */

  this.VNPI_highlightKey = function(isHovered, isPressed) {
    var tds = this.VNPI_keyboard.tBodies[0].getElementsByTagName('tr')[1];
    var keyrow = tds.getElementsByTagName('tr')[self.VNPI_hoverRow].getElementsByTagName('td');
    var className = [];

    //keyrow[self.VNPI_hoverCell].className = keyrow[self.VNPI_hoverCell].className.replace(/ ?(hover|pressed) ?/g, "");
    if (isPressed)
      className.push("pressed");
    if (isHovered)
      className.push("hover");
    keyrow[self.VNPI_hoverCell].className = className.join(" ");
  }


  this.VNPI_keyUpNav = function(e) {
    if (e.keyCode == 32) { //Space Bar
      this.VNPI_highlightKey(true, false);
    }
    return false;
  }
   
  this.VNPI_keynav = function(e) {
    if (e.keyCode == 9 || e.key == "Tab") { // Tab
      self.VNPI_close();
    }
    if ((e.keyCode == 32) || ((e.keyCode >= 37) && (e.keyCode <= 40))) { // Space and Arrow keys
      this.VNPI_highlightKey(false, false);

      var newRow = self.VNPI_hoverRow;
      var newColumn = self.VNPI_hoverCell;
      var isPressed = false;

      if (e.keyCode == 32) { //Space Bar
        var tds = this.VNPI_keyboard.tBodies[0].getElementsByTagName('tr')[1];
        var keyrow = tds.getElementsByTagName('tr')[self.VNPI_hoverRow].getElementsByTagName('td');
        this.VNPI_insert(keyrow[newColumn].innerHTML);
        isPressed = true;
      }
      if ((e.keyCode == 37) || (e.keyCode == 39)) { // Left/Right Arrows
        newColumn = (e.keyCode == 37) ? newColumn-1 : newColumn+1;
        if (newColumn < 0)
          newColumn = (newRow==3) ? 1 : 3;
        if ((newColumn > 3) || ((newRow==3) && (newColumn > 1)))
          newColumn = 0;
      }
      if ((e.keyCode == 38) || (e.keyCode == 40)) { // Up/Down Arrows
        if (newRow == 3) {
          newColumn = (newColumn < 1) ? 1 : 3;
        }
        newRow = (e.keyCode == 38) ? newRow-1 : newRow+1;
        newRow = (newRow < 0) ? 3 : (newRow > 3) ? 0 : newRow;
        if (newRow == 3) {
          newColumn = (newColumn < 3) ? 0 : 1;
        }
      }

      self.VNPI_hoverCell = newColumn;
      self.VNPI_hoverRow = newRow;
      this.VNPI_highlightKey(true, isPressed);

      if (e.stopPropagation) {
        e.stopPropagation();
        e.preventDefault();
      } else
        e.cancelBubble = true;
    }
    return false;
  };

  /* ***** Private functions *************************************** */
  function VNPI_addListener(elem, type, func, cap) {
    if (elem.addEventListener) {
      elem.addEventListener(type, function(e) { func.call(elem, e); }, cap);
    } else if (elem.attachEvent)
      elem.attachEvent('on' + type, function() { func.call(elem); });
  }

  function VNPI_findPos(obj) {
    var curleft = curtop = 0, scr = obj;
    while ((scr = scr.parentNode) && scr != document.body) {
      curleft -= scr.scrollLeft || 0;
      curtop -= scr.scrollTop || 0;
    }
    do {
      curleft += obj.offsetLeft;
      curtop += obj.offsetTop;
    } while (obj = obj.offsetParent);
    return [curleft, curtop];
  }

  function VNPI_innerDimensions() {
    if (self.innerHeight) {
      return [self.innerWidth, self.innerHeight];
    } else if (document.documentElement && document.documentElement.clientHeight) {
      return [document.documentElement.clientWidth, document.documentElement.clientHeight];
    } else if (document.body)
      return [document.body.clientWidth, document.body.clientHeight];
    return [0, 0];
  }

  function VNPI_scrollDist() {
    var html = document.getElementsByTagName('html')[0];
    if (html.scrollTop && document.documentElement.scrollTop) {
      return [html.scrollLeft, html.scrollTop];
    } else if (html.scrollTop || document.documentElement.scrollTop) {
      return [html.scrollLeft + document.documentElement.scrollLeft, html.scrollTop + document.documentElement.scrollTop];
    } else if (document.body.scrollTop)
      return [document.body.scrollLeft, document.body.scrollTop];
    return [0, 0];
  }

  function VNPI_getStyle(obj, styleProp) {
    if (obj.currentStyle) {
      var y = obj.currentStyle[styleProp];
    } else if (window.getComputedStyle)
      var y = window.getComputedStyle(obj, null)[styleProp];
    return y;
  }


  VNPI_addListener(window, 'resize', this.VNPI_position, false);
  VNPI_addListener(window, 'scroll', this.VNPI_position, false);
  this.VNPI_kbsize();
  VNPI_addListener(window, 'load', VNPI_buildKeyboardInputs, false);
})();

function VNPI_inputBuilder() {
  var inputElems = [
    document.getElementsByTagName('input'),
    document.getElementsByTagName('textarea')
  ];
  for (var x = 0, elem; elem = inputElems[x++];)
    for (var y = 0, ex; ex = elem[y++];)
      if ((ex.type == "number") || (ex.className.indexOf("numpadInput") > -1))
        VNPI_attach(ex);

  //VNPI_addListener(document.documentElement, 'click', function(e) { self.VNPI_close(); }, false);
  if (document.documentElement.addEventListener) {
    document.documentElement.addEventListener('click', function(e) { self.VNPI_close(); }, false);
  } else if (document.documentElement.attachEvent)
    document.documentElement.attachEvent('onclick', function() { self.VNPI_close(); });
}
VNPI_inputBuilder();