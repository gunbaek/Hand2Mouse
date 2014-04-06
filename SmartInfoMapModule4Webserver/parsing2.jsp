<style type="text/css">
<!--
.input {
	text-align: center;
	vertical-align: middle;
	margin: 0px;
	padding: 0px;
	height: 100px;
	width: 100px;
}
-->
</style>
<%@ page language="java" contentType="text/html; charset=UTF-8"pageEncoding="UTF-8" session="false"
import="java.net.*, 
	javax.xml.parsers.*,
	org.xml.sax.*, 
	org.w3c.dom.*, 
	java.util.*, 
	java.net.URLDecoder, 
	org.jsoup.Jsoup, 
	org.jsoup.nodes.Document, 
	org.jsoup.nodes.Element, 
	org.jsoup.select.Elements, 
	java.io.BufferedReader, 
	java.io.InputStreamReader, 
	java.net.URL, 
	org.json.JSONObject,
	org.json.JSONArray" %>

<%
String _countryName = new String(request.getParameter("c").getBytes("8859_1"),"euc-kr");
String countryName = URLDecoder.decode(_countryName,"utf-8");
String BaseUrl = "http://openapi.naver.com/search?target=encyc&key=dde56314298082d0205b2ad1960e9d1d&start=1&display=1&query=" + countryName;
String msg, msg2, linkAddr;

	HttpURLConnection conn = null;
	StringBuilder builder = new StringBuilder();

	try {

		
		DocumentBuilderFactory docBulFactory = DocumentBuilderFactory.newInstance();
		DocumentBuilder docBuilder = docBulFactory.newDocumentBuilder();
		org.w3c.dom.Document document = docBuilder.parse(BaseUrl);
		
		NodeList nList = document.getDocumentElement().getElementsByTagName("item");
		
		
		org.w3c.dom.Element element = (org.w3c.dom.Element)nList.item(0);
		NodeList list = element.getElementsByTagName("link");
		org.w3c.dom.Element cElement = (org.w3c.dom.Element)list.item(0);
		linkAddr = cElement.getFirstChild().getNodeValue();
	
		Document doc = Jsoup.connect(linkAddr).get();
		Elements rcw = doc.select("div.box_rlist div.lst_rlist ul#relatedContentsTop > li ul li");

		for(int i=1 ; i<rcw.size() ; i++){
			String BaseUrl2 = "http://openapi.naver.com/search?target=encyc&key=dde56314298082d0205b2ad1960e9d1d&start=1&display=1&query='" + countryName +"의 " + rcw.get(i).attr("title") + "'";

			DocumentBuilderFactory docBulFactory2 = DocumentBuilderFactory.newInstance();
			DocumentBuilder docBuilder2 = docBulFactory2.newDocumentBuilder();
			org.w3c.dom.Document document2 = docBuilder2.parse(BaseUrl2);
			
			NodeList nList2 = document2.getDocumentElement().getElementsByTagName("item");

			org.w3c.dom.Element element2 = (org.w3c.dom.Element)nList2.item(0);
			NodeList list2 = element2.getElementsByTagName("link");
			org.w3c.dom.Element cElement2 = (org.w3c.dom.Element)list2.item(0);
			String linkAddr2 = cElement2.getFirstChild().getNodeValue();
			
			Document doc2 = Jsoup.connect(linkAddr2).get();
			Elements rcw2 = doc2.select("p.txt");

			if(rcw2.size() > 0){
				String tmp = rcw.get(i).attr("title");
				
				out.println("<input class='input' type='button' value='"+rcw.get(i).attr("title")+"' onclick=\"change('"+tmp+"')\" />");
				
				out.println("<div style='display:none' id='" + rcw.get(i).attr("title") + "'>");
				out.println(rcw2.get(0));
				out.println("</div>");
			}
			if(i == 9) break;
			//
		}
	} catch (Exception e) {
		out.println("error:"+e);
	} finally {
		
	}
%>
<script>
function change(changeStr){
	document.getElementById(changeStr).style.display = "block";
}
</script>