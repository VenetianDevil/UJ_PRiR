import java.util.Random;

public class Main {
    public static void main(String[] argv) {
        RAID controller = new RAID();

        controller.addDisk(new Disk());
        controller.addDisk(new Disk());
        controller.addDisk(new Disk());
        controller.addDisk(new Disk());
        controller.addDisk(new Disk()); //kontrolny

        controller.startRAID();

        int logicDiskSize = controller.size();
        int[] randomSectors = new Random().ints(20, 0, logicDiskSize).toArray();

        for (int sectorK : randomSectors ) {
            RAIDInterface.RAIDState state = controller.getState();
            System.out.println("m - " + state);
            if (state == RAIDInterface.RAIDState.DEGRADED){
                controller.replaceDisk(new Disk());
            }
            System.out.println("m - writing 333 to sector: " + sectorK);
            controller.write(sectorK, 333);
            System.out.println("m - " + controller.getState());
            int read = controller.read(sectorK);
            System.out.println("m - " + sectorK + " " + read);
            if (read == 0) break;
        }
    }
}
