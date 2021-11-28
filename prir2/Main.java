import java.util.Random;
import java.util.concurrent.atomic.AtomicIntegerArray;

public class Main {
    public static void main(String[] argv) throws InterruptedException {
        RAID controller = new RAID();

        controller.addDisk(new Disk());
        controller.addDisk(new Disk());
        controller.addDisk(new Disk());
        controller.addDisk(new Disk());
        controller.addDisk(new Disk()); //kontrolny

        controller.startRAID();

        Thread t1 = new Thread(){
            public void run(){
            System.out.println("T1");
            int read = controller.read(10);
            System.out.println("m - " + 10 + " " + read);
            System.out.println("m - writing 333 to sector: " + 10);
            controller.write(10, 333);
            }
        };

        Thread t2 = new Thread(){
            public void run(){
                System.out.println("T2");
                System.out.println("m - writing 222 to sector: " + 20);
                controller.write(20, 222);
                int read = controller.read(20);
                System.out.println("m - " + 20 + " " + read);
                }
        };

        Thread t3 = new Thread(){
            public void run(){
                System.out.println("T3");
                while( controller.getState() == RAIDInterface.RAIDState.NORMAL) {
                    //
                }
                System.out.println("m - degreded disk = " + controller.degradedDiscIdx);
                controller.replaceDisk(new Disk());
            }
        };

        Thread t4 = new Thread(){
            public void run(){
                System.out.println("T4");
                System.out.println("m - writing 444 to sector: " + 22);
                controller.write(22, 444);
                int read = controller.read(22);
                System.out.println("m - " + 22 + " " + read);
            }
        };

        t1.start();
        t2.start();
        t3.start();
        t4.start();

        t1.join();
        t2.join();
        t3.join();
        t4.join();


//        System.out.println("shutdown");
//        controller.shutdown();
//        controller.write(30, 333);
//        int read = controller.read(30);

        System.out.println("KONIEC");
        System.out.println("degreded disk = " + controller.degradedDiscIdx);

        for (DiskInterface disk :controller.disks){
            AtomicIntegerArray sectors = disk.getSectors();
            for (int i =0; i < disk.size() ; i++){
                System.out.print(sectors.get(i) + "; ");
            }
            System.out.println();
        }
    }
}
