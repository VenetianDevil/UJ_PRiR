import java.rmi.RemoteException;
import java.util.List;

public class Oramcio implements PolygonalChainProcessor {
    @Override
    public int getConcurrentTasksLimit() throws RemoteException {
        return 1;
    }

    @Override
    public int process(String name, List<Position2D> polygonalChain) throws RemoteException {
        System.out.println("processing for " + name);
        for (Position2D point: polygonalChain) {
            System.out.println("\t\t\t\t\t" + name + " col: " + point.getCol() + " row: " + point.getRow());
        }
        try {
            Thread.sleep(5000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        return polygonalChain.size();
    }
}
