import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;

public class Main {

    public static void main(String[] argv) throws InterruptedException {
        try {
            Registry registry = java.rmi.registry.LocateRegistry.createRegistry(1099);
            PolygonalChainProcessor s = (PolygonalChainProcessor) UnicastRemoteObject.exportObject(new Oramcio(), 0);
            registry.rebind("ORAMCIO", s);
        } catch (RemoteException e) {
            e.printStackTrace();
        }

       Start start = new Start();
       PolygonalChain service;

        try {
            Registry registry = LocateRegistry.getRegistry(1099);
            service = (PolygonalChain) registry.lookup("POLYGONAL_CHAIN");
            service.setPolygonalChainProcessorName("//127.0.0.1:1099/ORAMCIO");

//            System.out.println(service);

            Thread t1 = new Thread(() -> {
                try {
                    service.newPolygonalChain("Ala", new Position2D(0,0), new Position2D(10, 10));
                    service.addLineSegment("Ala", new Position2D(0, 0), new Position2D(2, 5));
                    service.addLineSegment("Ala", new Position2D(6, 5), new Position2D(10, 10));
                    service.addLineSegment("Jadzia", new Position2D(6, 5), new Position2D(10, 10));
                    service.addLineSegment("Ala", new Position2D(2, 5), new Position2D(4, 7));
                    System.out.println("Ala = " + service.getResult("Ala"));
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            });

            Thread t2 = new Thread(() -> {
                try {
                    service.newPolygonalChain("Basia", new Position2D(0,0), new Position2D(10, 10));
                    service.addLineSegment("Basia", new Position2D(0, 0), new Position2D(2, 5));
                    service.addLineSegment("Jadzia", new Position2D(4,7), new Position2D(9, 2));
                    service.addLineSegment("Wiola", new Position2D(2,5), new Position2D(9, 2));
                    service.addLineSegment("Ala", new Position2D(9, 2), new Position2D(6, 5));
                    service.addLineSegment("Jadzia", new Position2D(2, 5), new Position2D(4, 7));
                    System.out.println("Basia = " + service.getResult("Basia"));
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            });


            Thread t3 = new Thread(() -> {
                try {
                    service.newPolygonalChain("Jadzia", new Position2D(0,0), new Position2D(10, 10));
                    service.addLineSegment("Jadzia", new Position2D(0, 0), new Position2D(2, 5));
                    service.addLineSegment("Basia", new Position2D(2, 5), new Position2D(10, 10));
                    service.addLineSegment("Wiola", new Position2D( 5, 8), new Position2D(16, 5));
                    System.out.println("Jadzia = " + service.getResult("Jadzia"));
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            });

            Thread t4 = new Thread(() -> {
                try {
                    service.newPolygonalChain("Wiola", new Position2D(0,9), new Position2D(60, 7));
                    service.addLineSegment("Wiola", new Position2D(16, 5), new Position2D(2, 5));
                    service.addLineSegment("Jadzia", new Position2D(9, 2), new Position2D(6, 5));
                    service.addLineSegment("Wiola", new Position2D( 9, 2), new Position2D(60, 7));
                    service.addLineSegment("Ala", new Position2D(4,7), new Position2D(9, 2));
                    service.addLineSegment("Wiola", new Position2D(0, 9), new Position2D(5, 8));
                    System.out.println("Wiola = " + service.getResult("Wiola"));
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            });

            t1.start();
            t2.start();
            t3.start();
            t4.start();

            t1.join();
            t2.join();
            t3.join();
            t4.join();

        } catch (NotBoundException | RemoteException e ){
            e.printStackTrace();
        }

//        System.exit(0);

    }
}