import java.util.ArrayList;
import java.util.Random;
import java.util.concurrent.atomic.AtomicIntegerArray;

public class Disk implements DiskInterface {
    AtomicIntegerArray diskSectors = new AtomicIntegerArray(10);
//    int[] diskSectors;
    int i = 0;

    public Disk(){
        for (int i = 0; i<diskSectors.length(); i++) {
            diskSectors.set(i, 1);
        }
//        for (int val : diskSectors ) {
//            System.out.print(val + "; ");
//        }
//        System.out.println();
    }

    public AtomicIntegerArray getSectors() {
        return diskSectors;
    }

    @Override
    public void write(int sector, int value) throws DiskError {
        System.out.println("\td - writing " + value + " to sector " + sector);
//        if (i++ % 5 == 0) {
//            throw new DiskError();
//        }

        diskSectors.set(sector, value);
    }

    @Override
    public int read(int sector) throws DiskError {
//        System.out.println("\td - reading " + diskSectors[sector] +  " from sector " + sector);

        i++;
        if (i == 11){
            System.out.println("\td - throwing error");
            throw new DiskError();
        }
        return diskSectors.get(sector);
    }

    @Override
    public int size() {
        return diskSectors.length();
    }
}
