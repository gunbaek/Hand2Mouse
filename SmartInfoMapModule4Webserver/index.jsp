<!DOCTYPE html>
<html>
  <head>
    <title>CSE Capstone project D team</title>
    <meta name="viewport" content="initial-scale=1.0, user-scalable=no">
    <meta charset="utf-8">
    <style>
      html, body, #map-canvas {
        height: 100%;
        margin: 0px;
        padding: 0px
      }
    </style>
    <script src="https://maps.googleapis.com/maps/api/js?v=3.exp&sensor=false"></script>
<script>
//글로벌 변수 선언
var map;
var myStyle;
var markers= [];
var geocoder;
var mouseClickedLatLng;
var infowindow, infowindow2;
var chartBase;
var country;
var contentString;
var xhr;
var resstr;
var xhr2;
var resstr2;
var layer;
var kmlLayer;

//기존의 마커를 없애고 새로운 마커를 생성하는 함수
function addMarker(location) {
	//기존의 마커들을 전부 삭제
	for (var i = 0; i < markers.length; i++) {
		markers[i].setMap(null);
	}
	markers = [];
	
	//새로운 마커 생성
	var marker = new google.maps.Marker({
		position: location,
		map: map
	});
	markers.push(marker);	
}

//Latlng를 이용해 나라를 찾는 함수
 function getCountry(results) {
	 var geocoderAddressComponent,addressComponentTypes,address;
	 for (var i in results) {
	   geocoderAddressComponent = results[i].address_components;
	   for (var j in geocoderAddressComponent) {
		 address = geocoderAddressComponent[j];
		 addressComponentTypes = geocoderAddressComponent[j].types;
		 for (var k in addressComponentTypes) {
		   if (addressComponentTypes[k] == 'country') {
			 return address;
		   }
		 }
	   }
	 }
	return 'Unknown';
  }
  
//나라를 찾아서 해당하는 나라의 국기를 얻어옴
function getCountryIcon(country){
	return chartBase + 'd_simple_text_icon_left&chld=' +
		country.long_name  + '|24|999|flag_' +
		country.short_name.toLowerCase() + '|24|000|FFF';
		//국기의 사이즈는 12, 16, 24 중 하나를 선택해야함 (google 제공 사이즈)
		//chld=<text>|<font_size>|<font_fill_color>|<icon_name>|<icon_size>|<icon_fill_color>|<icon_and_text_border_color>
}

function ajaxSend(searchCountry){

	if(window.ActiveXObject){
		xhr = new ActiveXObject("Microsoft.XMLHTTP");
	}else{
		xhr = new XMLHttpRequest();
	}

	xhr.onreadystatechange = res;

	xhr.open("get", "parsing.jsp?c="+searchCountry, false);
	xhr.setRequestHeader("Content-Type","application/x-www-form-urlencoded");
	xhr.send(null);
}
function res(){
	if(xhr.readyState == 4) {
		if(xhr.status == 200) {
			resstr = xhr.responseText;
		}
	}
}
function ajaxSend2(searchCountry){

	if(window.ActiveXObject){
		xhr2 = new ActiveXObject("Microsoft.XMLHTTP");
	}else{
		xhr2 = new XMLHttpRequest();
	}

	xhr2.onreadystatechange = res2;

	xhr2.open("get", "parsing2.jsp?c="+searchCountry, false);
	xhr2.setRequestHeader("Content-Type","application/x-www-form-urlencoded");
	xhr2.send(null);
}
function res2(){
	if(xhr2.readyState == 4) {
		if(xhr2.status == 200) {
			resstr2 = xhr2.responseText;
		}
	}
}


function addBorder(){
	kmlLayer = new google.maps.FusionTablesLayer({
    query: {
      select: 'geometry',
      from: '1UAmpArG1qvj2706di_OasNTbkHFb4bKiqVU_jKU'
    }
  });	
}
function initialize() {
	geocoder= new google.maps.Geocoder();
	infowindow = new google.maps.InfoWindow();
	infowindow2 = new google.maps.InfoWindow();
	chartBase = 'https://chart.googleapis.com/chart?chst=';
	
	//초기 옵션 부여
	var mapOptions = {
		zoom: 4,
		minZoom: 3,
		center: new google.maps.LatLng(17.644022,85.869141),
		panControl: false,
		zoomControl: true,
		zoomControlOptions: {
        	style: google.maps.ZoomControlStyle.LARGE,
        	position: google.maps.ControlPosition.LEFT_CENTER},
		scaleControl: true,
		mapTypeControl: false,
		streetViewControl: false,
		overviewMapControl: false,
		mapTypeId: google.maps.MapTypeId.HYBRID		
  	};
	
	//맵 생성
	map = new google.maps.Map(document.getElementById('map-canvas'),mapOptions);

	//맵 스타일 구성
	myStyle = [
	  {
		"featureType": "administrative.country",
		"elementType": "geometry",
		"stylers": [
		  { "color": "#ffffff" }
		]
	  },{
	  }
	];
	// 스타일 설정
	map.setOptions({styles:myStyle});
	
	
	// 클릭 리스너
	google.maps.event.addListener(map, 'click', function(mouseEvent) {														 
		map.setCenter(mouseEvent.latLng);
		//addMarker(mouseEvent.latLng);
		mouseClickedLatLng = mouseEvent.latLng;
		
		geocoder.geocode({'latLng': mouseClickedLatLng}, function(results, status) {
			if (status == google.maps.GeocoderStatus.OK) {
				if (results[1]) {
					addMarker(mouseClickedLatLng);
					country = getCountry(results);
					
					//ajax로 parsing 페이지로 정보 전송
					ajaxSend(country.long_name);
					ajaxSend2(country.long_name);
					//infoWindow 관련 
					contentString = '<div id="change_div" onclick="infowindowClose()" align="center">' + '<h1><img src="https://chart.googleapis.com/chart?chst=d_simple_text_icon_left&chld=|14|999|flag_'+
					  country.short_name.toLowerCase() + '|24|000|FFF" align="baseline">' + country.long_name + '</h1></div>' +
					'<div id="change_div2" onclick="infowindowClose()">' +
						resstr +
					'<p></div>' + '<div id="change_div3" align="center">' +
						resstr2 +
					'</div>';
					markers[0].setIcon(getCountryIcon(country));
					infowindow.setContent(contentString);
					infowindow.open(map, markers[0]);

					//map.panBy(500,800);
				
					//kml layer 생성 밑 설정
					if(layer != null){
						layer.setMap(null);
						delete layer;
					}
					layer = new google.maps.FusionTablesLayer({
						clickable:'false',
						suppressInfoWindows: 'false',
						query: {
							select: 'geometry',
							from: '1UAmpArG1qvj2706di_OasNTbkHFb4bKiqVU_jKU',
							where: "alpha2 = '"+country.short_name+"'"					},
						styles: [{
							polygonOptions: {
								fillColor: '#ffff00',
								fillOpacity: 0.5,
								borderColor: '#4c1130',
								borderOpacity: 0.5
					  		}
						}]
				 	 });
					layer.setMap(map);
					
				} else {
					
				}
			} else if (status == google.maps.GeocoderStatus.ZERO_RESULTS) {
			  
			} else if (status == google.maps.GeocoderStatus.OVER_QUERY_LIMIT) {

			}
		});//end of geocoder
	});//end of Listener
}
function infowindowClose(){
	infowindow.close();
}

function change(changeStr){
	document.all("change_div2").innerHTML=document.all(changeStr).innerHTML;
	
}
//로드 되었을때 초기화
google.maps.event.addDomListener(window, 'load', initialize);
</script>
  </head>
  <body>
    <div id="map-canvas"></div>
  
  </body>
</html>