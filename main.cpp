#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <boost/regex.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/convert.hpp>
#include <boost/convert/lexical_cast.hpp>
#include <boost/process.hpp>
#include <boost/thread/thread.hpp>


using namespace std;
using namespace boost::gregorian;
using namespace boost::filesystem;
using namespace boost::asio ;
using namespace boost::process;

/**
 * Cette propriété représente l'url pour le téléchargement de l'image Bing
 */
const static string url = "http://www.bing.com/HPImageArchive.aspx?format=js&idx=0&n=1&mkt=en-US";
/**
 * La date d'aujourd'hui, cette méthode sert à la propriété static today
 * @return
 */
string tdate () {
    using boost::gregorian::date;
    //using boost::date_time::day_clock;
    date today = date(day_clock::local_day()) ;
    //int jour, mois, annee ;
    //jour = today.day() ;
    string mois = "" ;
    if (today.month() < 10){
        mois = "0"+to_string(today.month());
    }else{
        mois = to_string(today.month());
    }
    string jour = "";
    if(today.day() < 10){
        jour = "0"+to_string(today.day());
    }else{
        jour = to_string(today.day());
    }
    string dateAujourdhui = std::to_string(today.year())+ mois + jour ;
    return dateAujourdhui;
}
/**
 * La date d'aujourd'hui
 */
string today = tdate();

/**
 * Cette Méthode détermine si l'image du jour a été téléchargée.
 * @return uri of downloaded image or null
 */
string bgDownload() {
    string bgDownloaded = "" ;
    try {
        path p("/usr/share/images/desktop-base/") ;
        string xp = ".*"+tdate()+".*";
        boost::regex expression(xp);
        for (directory_entry& x : directory_iterator(p)) {
            if (boost::regex_search(x.path().string(), expression)){
                bgDownloaded = x.path().string() ;

                break ;
            }
        }
    }catch(filesystem_error& e){
        cout << e.what() << endl ;
    }
    return bgDownloaded ;
}
/**
*
*/
bool downloadImage(){

    string urlImgDuJour = "" ;
    string nomDuFichierACreer = "" ;
    boost::asio::io_context context ;
    boost::asio::ip::tcp::resolver resolver{context} ;
    boost::asio::ip::tcp::socket socket{context} ;
    auto const results = resolver.resolve("www.bing.com","80");
    boost::asio::connect(socket,results.begin(), results.end());

    //requete http
    boost::beast::http::request<boost::beast::http::string_body> req{boost::beast::http::verb::get, "/HPImageArchive.aspx?format=js&idx=0&n=1&mkt=en-US", 11};
    req.set(boost::beast::http::field::host, "www.bing.com");
    req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    boost::beast::http::write(socket, req) ;
    boost::beast::flat_buffer buffer ;

    boost::beast::http::response<boost::beast::http::string_body> res ;
    boost::beast::http::read(socket,buffer, res) ;

    boost::regex expression("\"url\":\"(.+)?\",\"urlbase\":\".*\"");
    boost::smatch what;
    boost::regex_search(res.body(), what, expression);
    urlImgDuJour = what[1].str() ;

    boost::regex expression1("^.*/(.*)$");
    boost::smatch what1;
    boost::regex_search(urlImgDuJour, what1, expression1);
    nomDuFichierACreer = "/usr/share/images/desktop-base/"+today+"-"+what1[1].str();

    boost::beast::http::request<boost::beast::http::string_body> req2{boost::beast::http::verb::get, urlImgDuJour, 11};
    req2.set(boost::beast::http::field::host, "www.bing.com");
    req2.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    boost::beast::http::write(socket, req2) ;
    boost::beast::flat_buffer buffer2 ;

    boost::beast::http::response<boost::beast::http::string_body> res2 ;
    boost::beast::http::read(socket,buffer, res2) ;
    std::ofstream imgDuJour(nomDuFichierACreer, ios::out | ios::binary) ;
    imgDuJour << res2.body() ;
    imgDuJour.flush() ;
    cout << "juste apres le flush" << endl ;
    imgDuJour.close();
    return true;
}
string getLastImage(){

    ipstream pipe_stream;
    child c("xfconf-query -c xfce4-desktop -p /backdrop/screen0/monitor0/workspace0/last-image", std_out > pipe_stream);
    std::string line;
    std::string retour = "" ;
    while (pipe_stream && std::getline(pipe_stream, line) && !line.empty()){
        //std::cerr << line << std::endl;
        retour += line ;
    }
    c.wait();
    return retour ;
}
/**
* Xfce Bing wallpaper v1.0 2018
* 
*/
int main()
{
    // TODO Auto-generated method stub
    //Verifie si l'image du jour existe deja
    if(bgDownload() == "") {
        //Sinon telecharge l'image du jour, et reessaie 15 min apres
        //en cas d'echec
        while (downloadImage() != true) {
            try {
                cout << "Downloading...";
                //Thread.sleep(1000*60*15); //15 minutes
                boost::this_thread::sleep(boost::posix_time::milliseconds(1000*60*15));
            } catch (exception e) {
                // TODO Auto-generated catch block
                cout << "une exception s'est produite voir ligne 147" << endl ;
            }
        }
    }else {
        cout << "Vous avez deja l'image du jour" << endl ;
        //System.exit(0);
    }
    //Maintenant il faut savoir si l'image du jour est celle qui est définie avec xconf
    //Si l'image definie est différente de l'image du jour
    if( ! ( getLastImage() == bgDownload() )) {
        //je dois definir l'image du jour et quitter le programme
        try {
            std::string commande = "xfconf-query -c xfce4-desktop -p /backdrop/screen0/monitor0/workspace0/last-image -s "+bgDownload() ;
            ipstream pipe_stream;
            child c(commande);
            c.wait();
            cout << "Fond d'écran du jour défini avec succes";
            return 0;
        } catch (exception e) {
            // TODO Auto-generated catch block
            cout << "Impossible de definir le fond d'écran, veuillez contactez deressources@gmail.com pour signaler le bug";
            return 1;
        }
    }else {
        cout << "Le fond d'écran est déjà défini le programme se termine !" << endl ;
        return 0;
    }
}
