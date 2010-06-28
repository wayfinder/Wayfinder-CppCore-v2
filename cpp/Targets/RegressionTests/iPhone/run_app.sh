executionpath=$1
sysctl -w security.mac.proc_enforce=0 
sysctl -w security.mac.vnode_enforce=0 

mkdir -p /var/root/Documents
cd $executionpath 
pwd
ls
rsync -r RegressionTester/nav2data .
./RegTestLauncher
sysctl -w security.mac.proc_enforce=1
sysctl -w security.mac.vnode_enforce=1
