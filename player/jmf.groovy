


import javax.media.*;
import java.io.*;
import java.net.URL;

@GrabResolver(name='local', root='http://it-maven.echostar.com/content/groups/m2repo/')
@Grab('javax.media:jmf:2.1.1e')
import javax.media.*;
 
public class playmp3 {
    public static void main(String[] args) {
        Tmp3 player = new Tmp3(args[0]);
        player.start();
    }
}

class Tmp3 extends Thread {
 
    private URL url;
    private MediaLocator mediaLocator;
    private Player playMP3;
 
    public Tmp3(String mp3) {
        try {
            this.url = new URL(mp3);
        } catch(java.net.MalformedURLException e) {
            System.out.println(e.getMessage());
        }
    }

    public void run() {
        try {
            mediaLocator = new MediaLocator(url);     
            playMP3 = Manager.createPlayer(mediaLocator);
        } catch(java.io.IOException e) {
            System.out.println(e.getMessage());
        } catch(javax.media.NoPlayerException e) {
            System.out.println(e.getMessage());
        }

        if (playMP3 != null) {
            playMP3.addControllerListener(new ControllerListener() {
                public void controllerUpdate(ControllerEvent e) {
                    if (e instanceof EndOfMediaEvent) {
                        playMP3.stop();
                        playMP3.close();
                    }
                }
            });

            playMP3.realize();
            playMP3.start();
        }
    }
}
