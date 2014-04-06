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
String msg, linkAddr;

	HttpURLConnection conn = null;
	StringBuilder builder = new StringBuilder();

	try {
		URL url = new URL(BaseUrl);
		conn = (HttpURLConnection)url.openConnection();

		if (conn != null) {// 정상접속이 되었다면
			conn.setConnectTimeout(10000);// 최대 대기시간10초

			if (conn.getResponseCode() == HttpURLConnection.HTTP_OK) {
				// InputStreamReader 객체 얻어오기
				InputStreamReader isr =
				new InputStreamReader(conn.getInputStream());
				BufferedReader br = new BufferedReader(isr);

				// 반복문 돌면서 읽어오기
				while (true) {
					String line = br.readLine();
					if (line == null)
					break;
					// 읽어온 문자열을 객체에 저장
					builder.append(line);
				}
				br.close();
			}// if
		}// if
		msg = builder.toString();
		
		DocumentBuilderFactory docBulFactory = DocumentBuilderFactory.newInstance();
		DocumentBuilder docBuilder = docBulFactory.newDocumentBuilder();
		org.w3c.dom.Document document = docBuilder.parse(BaseUrl);
		
		NodeList nList = document.getDocumentElement().getElementsByTagName("item");
		
		
		org.w3c.dom.Element element = (org.w3c.dom.Element)nList.item(0);
		NodeList list = element.getElementsByTagName("link");
		org.w3c.dom.Element cElement = (org.w3c.dom.Element)list.item(0);
		linkAddr = cElement.getFirstChild().getNodeValue();
	
		
		
		Document doc = Jsoup.connect(linkAddr).get();
		Elements rcw = doc.select("div.wr_tmp_profile ul.tmp_profile li");
		
		String etc1 = "기타";
		String etc2 = "분야";
		String etc3 = "유형";
		for(int i=0 ; i<rcw.size() ; i++){
			if(etc1.equals(rcw.get(i).select("span").text())){
				continue;
			}else if(etc2.equals(rcw.get(i).select("span").text())){
				continue;
			}else if(etc3.equals(rcw.get(i).select("span").text())){
				continue;
			}
			if(i > 10){
				break;
			}
			out.println(rcw.get(i).select("span").text() );
			out.println(" : ");
			out.println(rcw.get(i).select("p").text() );
			out.println("<br>");
		}
	} catch (Exception e) {
		out.println("error:"+e);
	} finally {
		conn.disconnect(); // 접속 종료
	}
%>