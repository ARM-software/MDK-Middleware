/* Main function, that handles request and responses in background.
 * Response functions are handled if response code equals to OK 200. */
function updateMultiple(formUpd, callBack, userName, userPassword) {
 xmlHttp = GetXmlHttpObject();
 if(xmlHttp == null) {
  alert("XmlHttp not initialized!");
  return 0;
 }
 xmlHttp.onreadystatechange = responseHandler; 
 xmlHttp.open("GET", formUpd.url, true, userName, userPassword);
 xmlHttp.send(null);

 function responseHandler(){
  if(xmlHttp.readyState == 4) { //response ready 
   if(xmlHttp.status == 200) { //handle received data
    var xmlDoc = xmlHttp.responseXML;
    if(xmlDoc == null)
     return 0;
    try {  //catching IE bug
     processResponse(xmlDoc);
    }
    catch(e) {
     return 0;
    }
    /* Callback function for custom update. */
    if (callBack != undefined)
     callBack();
   }
   else if(xmlHttp.status == 401)
    alert("Error code 401: Unauthorized");
   else if(xmlHttp.status == 403)
    alert("Error code 403: Forbidden");
   else if(xmlHttp.status == 404)
    alert("Error code 404: URL not found!");
  }
 }
}

function processResponse(xmlDoc) {
 textElementArr = xmlDoc.getElementsByTagName("text");
 for(var i = 0; i < textElementArr.length; i++) {
  try {
   elId = textElementArr[i].childNodes[0].childNodes[0].nodeValue;
   elValue = textElementArr[i].childNodes[1].childNodes[0].nodeValue;
   document.getElementById(elId).value = elValue;
  }
  catch(error) {
   if(elId == undefined){
    continue;
   }
   else if(elValue == undefined) {
    elValue = "";
    document.getElementById(elId).value = elValue;
   }
  }
 }
 checkboxElementArr = xmlDoc.getElementsByTagName("checkbox");
 for(var i = 0; i < checkboxElementArr.length; i++) {
  try {
   elId = checkboxElementArr[i].childNodes[0].childNodes[0].nodeValue;
   elValue = checkboxElementArr[i].childNodes[1].childNodes[0].nodeValue;
   if(elValue.match("true"))
    document.getElementById(elId).checked = true;
   else
    document.getElementById(elId).checked = false;
  }
  catch(error) {
   if(elId == undefined) {
    continue;
   }
   else if(elValue == undefined) //we leave current state
    continue;
  }
 }
 selectElementArr = xmlDoc.getElementsByTagName("select");
 for(var i = 0; i < selectElementArr.length; i++) {
  try {
   elId = selectElementArr[i].childNodes[0].childNodes[0].nodeValue;
   elValue = selectElementArr[i].childNodes[1].childNodes[0].nodeValue;
   document.getElementById(elId).value = elValue;
   if(elValue.match("true"))
    document.getElementById(elId).selected = true;
   else
    document.getElementById(elId).selected = false;
  }
  catch(error) {
   if(elId == undefined) {
    continue;
   }
   else if(elValue == undefined) {
    elValue = "";
    document.getElementById(elId).value = elValue;
   }
  }
 }
 radioElementArr = xmlDoc.getElementsByTagName("radio");
 for(var i = 0; i < radioElementArr.length; i++) {
  try {
   elId = radioElementArr[i].childNodes[0].childNodes[0].nodeValue;
   elValue = radioElementArr[i].childNodes[1].childNodes[0].nodeValue;
   if(elValue.match("true"))
    document.getElementById(elId).checked = true;
   else
    document.getElementById(elId).checked = false;
  }
  catch(error) {
   if(elId == undefined) {
    continue;
   }
   else if(elValue == undefined) //we leave current state
    continue;
  }
 }
}

/* XMLHttpRequest object specific functions */
function GetXmlHttpObject() { //init XMLHttp object
 var xmlHttp=null;
 try {
  xmlHttp=new XMLHttpRequest(); // Firefox, Opera 8.0+, Safari
 }
 catch (e) {
  try {   // Internet Explorer
   xmlHttp=new ActiveXObject("Msxml2.XMLHTTP");
  }
  catch (e) {
   xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
  }
 }
 return xmlHttp;
}

/* Objects templates */
function periodicObj(url, period) {
 this.url = url;
 this.period = (typeof period == "undefined") ? 0 : period;
}
