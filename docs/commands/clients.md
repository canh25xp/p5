# p5 clients

## The problem

```sh
p4 clients --me
```

Like many p4 commands, `p4 clients` display too much information on 1 line

```
Client SRV_DSS_SECSETTINGS_BENI_VANCANH_NG 2026/01/07 root c:\Users\vancanh.ng\projects\SRV_DSS_SECSETTINGS_BENI_VANCANH_NG 'Created by vancanh.ng. '
Client SRV_DSS_SECSETTINGS_COOSA_VANCANH_NG 2026/01/08 root /home/vancanh-ng/projects/SecSettings_COOSA 'Created by vancanh.ng. '
Client VANCANH-NG_A15-EUR-OPEN_ONEUI80_MR202508_SYSTEM_BBREL 2026/01/26 root /mnt/wsl/shared/VANCANH-NG_A15-EUR-OPEN_ONEUI80_MR202508_SYSTEM_BBREL 'Created by vancanh.ng. //TOOLS/D4/a15_eur_open-oneui80-mr202508-system-bbrel/31933407 Created from TEMPLATE_D4_A15-EUR-OPEN_ONE'
Client VANCANH-NG_A15-EUR-OPEN_ONEUI80_MR202508_VENDOR_BBREL 2026/01/19 root /mnt/wsl/shared/VANCANH-NG_A15-EUR-OPEN_ONEUI80_MR202508_VENDOR_BBREL 'Created by vancanh.ng. //TOOLS/D4/a15_eur_open-oneui80-mr202508-vendor-bbrel/32196539 Created from TEMPLATE_D4_A15-EUR-OPEN_ONE'
Client swarm-6cf5aa8b-e057-0c87-4b96-35f7b798d70a 2025/04/08 root /opt/perforce/swarm/data/clients/76616E63616E682E6E67/swarm-6cf5aa8b-e057-0c87-4b96-35f7b798d70a 'Created by vancanh.ng. '
Client swarm-7253949c-de9c-d124-c97d-fe308aa3f28b 2025/08/15 root /opt/perforce/swarm/data/clients/76616E63616E682E6E67/swarm-7253949c-de9c-d124-c97d-fe308aa3f28b 'Created by vancanh.ng. '
```

## Propose changes:

```sh
$ p5 clients --me
```

```
SRV_DSS_SECSETTINGS_BENI_VANCANH_NG
  2026/01/07
  root c:\Users\vancanh.ng\projects\SRV_DSS_SECSETTINGS_BENI_VANCANH_NG
  Created by vancanh.ng.

SRV_DSS_SECSETTINGS_COOSA_VANCANH_NG
  2026/01/08
  root /home/vancanh-ng/projects/SecSettings_COOSA
  Created by vancanh.ng.

VANCANH-NG_A15-EUR-OPEN_ONEUI80_MR202508_SYSTEM_BBREL
  2026/01/26
  root /mnt/wsl/shared/VANCANH-NG_A15-EUR-OPEN_ONEUI80_MR202508_SYSTEM_BBREL
  Created by vancanh.ng. //TOOLS/D4/a15_eur_open-oneui80-mr202508-system-bbrel/31933407 Created from TEMPLATE_D4_A15-EUR-OPEN_ONE

VANCANH-NG_A15-EUR-OPEN_ONEUI80_MR202508_VENDOR_BBREL
  2026/01/19
  root /mnt/wsl/shared/VANCANH-NG_A15-EUR-OPEN_ONEUI80_MR202508_VENDOR_BBREL
  Created by vancanh.ng. //TOOLS/D4/a15_eur_open-oneui80-mr202508-vendor-bbrel/32196539 Created from TEMPLATE_D4_A15-EUR-OPEN_ONE

swarm-6cf5aa8b-e057-0c87-4b96-35f7b798d70a
  2025/04/08
  root /opt/perforce/swarm/data/clients/76616E63616E682E6E67/swarm-6cf5aa8b-e057-0c87-4b96-35f7b798d70a
  Created by vancanh.ng.

swarm-7253949c-de9c-d124-c97d-fe308aa3f28b
  2025/08/15
  root /opt/perforce/swarm/data/clients/76616E63616E682E6E67/swarm-7253949c-de9c-d124-c97d-fe308aa3f28b
  Created by vancanh.ng.
```
