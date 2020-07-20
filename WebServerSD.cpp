/**********************************************************************************
************************************BIBLIOTECAS************************************
**********************************************************************************/

#include "WebServerSD.h"

/**********************************************************************************
********************************FIM BIBLIOTECAS************************************
**********************************************************************************/

/*
 Web Server - HTTP Autentication
 Baseado na versão de exemplo do Arduino.

   - exemplo que mostra como incluir html, css,
   javascript e imagem em arquivos no sdcard e também
   como fazer download de um arquivo pdf do sdcard.

   by Fabiano A. Arndt (fabianoallex)
*/




/**********************************************************************************
*************************ROTINAS USUARIO E SENHA***********************************
***********************************************************************************/


boolean validar_usuario(char * linebuf) {

	char usuario_senha[] = "admin:admin"; //"admin:admin";
	byte t = strlen(usuario_senha);
	int tamanhoEnc = (((t-1) / 3) + 1) * 4; //tamanho da string codificada
	char *out = base64_encode(usuario_senha, t);
	char out2[tamanhoEnc];

	for (t=0; t<(tamanhoEnc); t++) {
		out2[t] = linebuf[21+t];
	}

	return (strstr(out2, out)>0);
}

/**********************************************************************************
*************************FIM ROTINA USUARIO E SENHA********************************
***********************************************************************************/



/**********************************************************************************
***********************************PAGINAS HTML************************************
***********************************************************************************/
EthernetServer * server;

void iniciar_ethernet_01(){
  byte ip[4]      = {192,168,1,50};                    //definir aqui o ip
  byte gateway[4] = {192,168,1,2};
  byte subnet[4]  = {255,255,255,0};
  byte mac[6]     = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  int  porta      = 80;

  server = new EthernetServer(porta);

  Ethernet.begin(mac, ip, gateway, subnet);         //caso necessario gateway utilizar essa linha
  server->begin();
}

void iniciar_ethernet_02(){
  byte ip[4]      = {192,168,1,50};                    //definir aqui o ip
  byte mac[6]     = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  int  porta      = 80;

  server = new EthernetServer(porta);

  Ethernet.begin(mac, ip);
  server->begin();
}

void iniciar_ethernet() {              //escolher apenas um dos dois modos de iniciar o ethernet shield
  iniciar_ethernet_01();               //inicia com ip, gateway, mascara e porta
  //iniciar_ethernet_02();             //inicia só com ip e porta
}

void html_cab_200_ok(EthernetClient &client){

  client.println(F("HTTP/1.1 200 OK\n"
                   "Content-Type: text/html\n"
                   "Connection: keep-alive\n\n"));
}


void html_logoff(EthernetClient &client){
    client.print(F(
                 "HTTP/1.1 401 Authorization Required\n"
                 "Content-Type: text/html\n"
                 "Connnection: close\n\n"
                 "<!DOCTYPE HTML>\n"
                 "<html><head><title>Logoff</title>\n"
                 "<script>document.execCommand('ClearAuthenticationCache', 'false');</script>"  //IE logoff
                 "<script>try {"                                                                //mozila logoff
                 "   var agt=navigator.userAgent.toLowerCase();"
                 "   if (agt.indexOf(\"msie\") != -1) { document.execCommand(\"ClearAuthenticationCache\"); }"
                 "   else {"
                 "     var xmlhttp = createXMLObject();"
                 "      xmlhttp.open(\"GET\",\"URL\",true,\"logout\",\"logout\");"
                 "     xmlhttp.send(\"\");"
                 "     xmlhttp.abort();"
                 "   }"
                 " } catch(e) {"
                 "   alert(\"erro ao fazer logoff\");"
                 " }"
                 "function createXMLObject() {"
                 "  try {if (window.XMLHttpRequest) {xmlhttp = new XMLHttpRequest();} else if (window.ActiveXObject) {xmlhttp=new ActiveXObject(\"Microsoft.XMLHTTP\");}} catch (e) {xmlhttp=false;}"
                 "  return xmlhttp;"
                 "}</script>"
                 "</head><body><h1>Voce nao esta mais conectado</h1></body></html>\n"));
}

void html_autenticar(EthernetClient &client) {
  client.print(F("HTTP/1.1 401 Authorization Required\n"
               "WWW-Authenticate: Basic realm=\"Area Restrita\"\n"
               "Content-Type: text/html\n"
               "Connnection: close\n\n"
               "<!DOCTYPE HTML>\n"
               "<html><head><title>Error</title>\n"
               "</head><body><h1>401 Acesso nao autorizado</h1></body></html>\n"));
}

void html_autenticado(EthernetClient &client, char * filename){
  client.println(F("HTTP/1.1 200 OK\n"
                   "Content-Type: text/html\n"
                   "Connection: keep-alive\n"));

  write_from_file2(client, filename);
}


void js_file(EthernetClient &client, char * filename){
  client.println(F("HTTP/1.1 200 OK\n"
                   "Content-Type: text/javascript\n"
                   "Connection: keep-alive\n"));

  write_from_file2(client, filename);
}

void css_file(EthernetClient &client, char * filename){
  client.println(F("HTTP/1.1 200 OK\n"
                   "Content-Type: text/css\n"
                   "Connection: keep-alive\n"));

  write_from_file2(client, filename);
}

void favicon_file(EthernetClient &client, char * filename){
  client.println(F("HTTP/1.1 200 OK\n"
                   "Content-Type: image/x-icon\n"
                   "Connection: keep-alive\n"));

  write_from_file2(client, filename);
}


void pdf_file_download(EthernetClient &client, char * filename){
  client.println(F("HTTP/1.1 200 OK\n"
                   "Content-Type: application/download\n"
                   "Connection: close\n"));

  write_from_file2(client, filename);
}

void jpg_file(EthernetClient &client, char * filename){
  client.println(F("HTTP/1.1 200 OK\n"
                   "Content-Type: file/jpg\n"
                   "Connection: close\n"));

  write_from_file2(client, filename);
}


void exec_ethernet() {
  EthernetClient client = server->available();
  if (client) {
	String nameFile = "idx.html";
    char linebuf[80];
    memset(linebuf, 0, sizeof(linebuf));

    int     charCount          = 0;
    int     type			   = 0;
    boolean autenticado        = false;
    boolean currentLineIsBlank = true;
    boolean logoff             = false;
    boolean indCss             = false;
    boolean indJs              = false;
    boolean indPdfDataSheet    = false;
    boolean indJpgUno          = false;
    boolean indFavicon         = false;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        linebuf[charCount] = c;

        if (charCount<sizeof(linebuf)-1) { charCount++; }
        //Serial.write(c);

        if (c == '\n' && currentLineIsBlank) {

          if (autenticado && !logoff ) {

        	// For Debug
        	//Serial.println(nameFile);

            if (indJs){
              js_file(client, nameFile.c_str());                          //js file
            } else if(indCss){
              css_file(client, nameFile.c_str());                          //js file
            } else if(indPdfDataSheet){
              pdf_file_download(client, nameFile.c_str());        //datasheet download
            } else if(indJpgUno){
              jpg_file(client, nameFile.c_str());                       //jpg file
            } else if(indFavicon){
              jpg_file(client, nameFile.c_str());                   //icone do browser
            } else {
              html_autenticado(client, nameFile.c_str());        //página inicial
            }
          } else {
            logoff ? html_logoff(client) : html_autenticar(client);
          }
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;

          String reqFull = String(linebuf);
          if(reqFull.indexOf("GET") != -1){
        	  reqFull = reqFull.substring(reqFull.indexOf("/")+1,reqFull.indexOf(" HTTP"));
        	  nameFile = reqFull;

          	  if(reqFull.indexOf(".css") != -1){
          		indCss = true;
          	  } else if (reqFull.indexOf(".js") != -1){
          		indJs = true;
          	  } else if (reqFull.indexOf(".jpg") != -1 || reqFull.indexOf(".png") != -1){
          		indJpgUno = true;
          	  } else if (reqFull.indexOf(".ico") != -1){
          		indFavicon = true;
          	  } else if (reqFull.indexOf(".pdf") != -1){
          		indPdfDataSheet = true;
          	  }
          }

          if (strstr(linebuf, "GET /logoff"         )>0 ) { logoff = true; }
          if (strstr(linebuf, "Authorization: Basic")>0 ) { if ( validar_usuario(linebuf) )   {  autenticado = true;   } }

          memset(linebuf, 0, sizeof(linebuf));
          charCount = 0;
        } else if (c != '\r') {
          currentLineIsBlank = false;    // you've gotten a character on the current line
        }
      }
    }

    delay(1);
    client.stop();
  }
}
/**********************************************************************************
**************************************** FIM PAGINAS HTML**************************
***********************************************************************************/


