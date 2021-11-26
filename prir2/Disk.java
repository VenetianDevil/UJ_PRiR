import java.util.ArrayList;
import java.util.Random;

public class Disk implements DiskInterface {
    int[] diskSectors;
    int i = 0;

    public Disk(){
        diskSectors = new Random().ints(5, 0, 50).toArray();
        for (int val : diskSectors ) {
            System.out.print(val + "; ");
        }
        System.out.println();
    }

    public int[] getSectors() {
        return diskSectors;
    }

    @Override
    public void write(int sector, int value) throws DiskError {
//        System.out.println("i = " + i);
        if (i++ % 5 == 0) {
            throw new DiskError();
        }

        diskSectors[sector] = value;
    }

    @Override
    public int read(int sector) throws DiskError {
        i++;
//        System.out.println("i = " + i);
        if (i == 15){
            throw new DiskError();
        }
        return diskSectors[sector];
    }

    @Override
    public int size() {
        return diskSectors.length;
    }
}
