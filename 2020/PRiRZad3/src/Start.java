import org.xml.sax.helpers.AttributeListImpl;

import java.net.MalformedURLException;
import java.rmi.*;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.*;
import java.util.concurrent.SubmissionPublisher;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Start extends UnicastRemoteObject implements RemoteConverterInterface{

    private static int counter = 0;
    private Vector<Integer> usersId;
    private Map<Integer, List<Integer>> userData;
    private static Map<Integer, List<Integer>> userResult;
    private static Map<Integer, Boolean> resultRead;
    private String url;
    private static Lock lock = new ReentrantLock();

    public Start() throws  RemoteException {
        usersId = new Vector<>();
        userData = new HashMap<>();
        userResult = new HashMap<>();
        resultRead = new HashMap<>();

    }

    @Override
    synchronized public int registerUser() throws RemoteException {
        usersId.add(counter);
        userData.put(counter, new ArrayList<>());
        userResult.put(counter, new ArrayList<>());
        resultRead.put(counter, false);
        counter++;
        return counter - 1;
    }

    @Override
    public void addDataToList(int userID, int value) throws RemoteException {
        userData.get(userID).add(value);
    }

    @Override
    public void setConverterURL(String url) throws RemoteException {
        this.url = url;
    }

    @Override
    public void endOfData(int userID) throws RemoteException {

        SendDataRunnable sendDataRunnable = new SendDataRunnable(userData.get(userID), url, userID);
        Thread thread = new Thread(sendDataRunnable, String.valueOf(userID));
        thread.start();
    }

    @Override
    public boolean resultReady(int userID) throws RemoteException {
        return resultRead.get(userID);
    }

    @Override
    public List<Integer> getResult(int userID) throws RemoteException {
        if(resultReady(userID))
        {
            return userResult.get(userID);
        }
        return null;
    }

    public static void main(String[] args) throws AccessException, RemoteException, AlreadyBoundException {
        try {
            Registry registry = LocateRegistry.getRegistry();
            registry.rebind("REMOTE_CONVERTER", new Start());
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    public static class SendDataRunnable implements Runnable {

        List<Integer> dataList;
        private String url;
        private int id;

        public SendDataRunnable(List<Integer> dataList, String url, int id)
        {
            this.dataList = dataList;
            this.url = url;
            this.id = id;
        }

        @Override
        public void run() {
            sendData();
        }

        private void sendData()
        {
            lock.lock();

            try {
                List<Integer> result;
                Remote r = java.rmi.Naming.lookup(url);
                ConverterInterface cri = (ConverterInterface) r;
                result = cri.convert(dataList);
                userResult.get(id).addAll(result);
                resultRead.put(id, true);
            } catch (RemoteException | NotBoundException | MalformedURLException e) {
                e.printStackTrace();
            } finally {
                lock.unlock();
            }
        }
    }
}
