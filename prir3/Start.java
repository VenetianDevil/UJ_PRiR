import java.net.MalformedURLException;
import java.rmi.NotBoundException;
import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Start {

    public Start() {
        try {
            Registry registry = java.rmi.registry.LocateRegistry.createRegistry(1099);
            PolygonalChain s = (PolygonalChain) UnicastRemoteObject.exportObject(new MyService(), 0);
            registry.rebind("POLYGONAL_CHAIN", s);
        } catch (RemoteException  e) {
            e.printStackTrace();
        }
    }
}

class MyService implements PolygonalChain {

    int processingLimit;
    ArrayList<Lock> locks = new ArrayList<>();

    PolygonalChainProcessor polygonalChainProcessor;
    ArrayList<Chain> chains = new ArrayList<>();

    @Override
//synchronized? czy wtedy nie pojawi sie problem ze mozemy probować juz dodać linie do krzywej, jak jeszcze jej nie zapisaliśmy
    public synchronized void newPolygonalChain(String name, Position2D firstPoint, Position2D lastPoint) throws RemoteException {
        if (chains.stream().anyMatch(c -> c.name.equals(name))){
            throw new RemoteException();
        }
        Chain chain = new Chain(name, firstPoint, lastPoint);
        chains.add(chain);
//        System.out.println("i have chains: " + chains.size());
    }

    @Override
    public void addLineSegment(String name, Position2D firstPoint, Position2D lastPoint) throws RemoteException {
        for (Chain chain : chains) {
            if (chain.name.equals(name)){
                int chainIdx = chains.indexOf(chain);
                synchronized (chains.get(chainIdx)){
                    chain.addLine(new Line(firstPoint, lastPoint));
                }
                synchronized (chains.get(chainIdx)){
                    if (chain.isChainComplete() && !chain.processed) {
//                        System.out.println(chain.name + " ready to process ");
                        Thread thread = new Thread(() -> {
                            try {
                                sendToProcess(chain);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        });
                        thread.start();
                    }
                }
                return;
            }
        }

        throw new RemoteException();
    }

    private void sendToProcess(Chain chain) throws InterruptedException {
        while(!chain.processed){
            for (Lock lock: locks) {
                if(lock.tryLock()){
                    try {
                        List<Position2D> polygonalChain = chain.getPolygonalChain();
//                        System.out.println("\t\t" + chain.name + " inside process");
                        chain.result = polygonalChainProcessor.process(chain.name, polygonalChain);
                        chain.processed = true;
                    }
                    catch (RemoteException e) {
                                e.printStackTrace();
                    } finally {
//                        System.out.println("\t\t\t\t" + chain.name + " finished process");
                        lock.unlock();
//                        System.out.println("unlock " + chain.name);
                    }
                    return;
                }
            }
        }
    }

    @Override
    public Integer getResult(String name) throws RemoteException {
        for (Chain chain : chains) {
            if (chain.name.equals(name)){
                int chainIdx = chains.indexOf(chain);
                synchronized (chains.get(chainIdx)){
//                    System.out.println(chain.name + " " + chain.processed + " get result: " + chain.result );
                    if (chain.processed) {
                        return chain.result;
                    }
                }
                break;
            }
        }
        return null;
    }

    @Override
    public void setPolygonalChainProcessorName(String uri) throws RemoteException {
        try {
            Remote r = java.rmi.Naming.lookup(uri);
            polygonalChainProcessor = (PolygonalChainProcessor) r;
            processingLimit = polygonalChainProcessor.getConcurrentTasksLimit();
            initializeLocks();
        } catch ( NotBoundException | MalformedURLException e ){
            e.printStackTrace();
            throw new RemoteException();
        }
    }

    private void initializeLocks(){
        for (int i = 0; i < processingLimit; i++){
            locks.add(new ReentrantLock());
        }
    }
}

class Line {
    private final Position2D a;
    private final Position2D b;

    public Line(Position2D firstPoint, Position2D lastPoint) {
        a = firstPoint;
        b = lastPoint;
    }

    public Position2D a(){
        return a;
    }

    public Position2D b(){
        return b;
    }
}

class Chain {
    String name;
    Position2D firstPoint;
    Position2D lastPoint;
    boolean processed = false;
    Integer result;
    volatile private ArrayList<Line> lines = new ArrayList<>();
    volatile private List<Position2D> polygonalChain = new ArrayList<>();

    Chain(String name, Position2D firstPoint, Position2D lastPoint) {
        this.name = name;
        this.firstPoint = firstPoint;
        this.lastPoint = lastPoint;
    }

    public void addLine(Line line){
        lines.add(line);
//        System.out.println("line added to " + name);
    }

    public boolean isChainComplete() {
        Position2D nextPoint = firstPoint;
//        zakladam ze pierwszy i ostatni punkt sa rozne i wgl ze punkty sie nie powtarzaja
        polygonalChain.add(nextPoint); //dodaje 1 punkt krzywej
        while (! nextPoint.equals(lastPoint)){
            boolean foundNext = false;
            for (Line line: lines) {
                if (line.a().equals(nextPoint)){
                    foundNext = true;
                    nextPoint = line.b();
                    break;
                }
            }
            if(!foundNext){
                polygonalChain.clear();
                return false;
            }
            polygonalChain.add(nextPoint);
        }

        return true;
    }

    public List<Position2D> getPolygonalChain() {

        return polygonalChain;
    }

}