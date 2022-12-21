using System;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;

namespace launcher.Services
{


    public interface IClientExecutionService
    {
        public void LockLauncher(Process process);
        public Task WaitForUnlockLauncher();
        public void ForceUnlockLauncher();

        public CancellationTokenSource CancellationTokenSource { get; }
    }

    public class ClientExecutionService : IClientExecutionService
    {
        private Process? lockedProcess;
        private CancellationTokenSource cancellationTokenSource = new CancellationTokenSource();

        CancellationTokenSource IClientExecutionService.CancellationTokenSource => cancellationTokenSource;

        public void ForceUnlockLauncher()
        {
            // Cancels any WaitForUnlockLauncher tasks
            //
            cancellationTokenSource.Cancel();

            // Kill and dispose the process
            //
            if(lockedProcess != null)
            {
                lockedProcess.Kill();
                lockedProcess.Dispose();
                lockedProcess = null;
            }
        }

        public void LockLauncher(Process process)
        {
            // Unlock the launcher prior locking
            ForceUnlockLauncher();

            // Create a new token source
            cancellationTokenSource = new CancellationTokenSource();

            // Assign the watched process
            lockedProcess = process;
        }

        public async Task WaitForUnlockLauncher()
        {
            // We don't wait for unlock if there's no locked process
            if (lockedProcess == null)
                return;

            // Wait for either token cancellation or process termination
            await lockedProcess.WaitForExitAsync(cancellationTokenSource.Token);
            return;
        }
    }
}
