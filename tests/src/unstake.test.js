import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, nano_models, txFromEtherscan} from './test.fixture';

// Test from replayed transaction: https://bscscan.com/tx/0xc64361abc3a86418d608c3c1b4ca8e3b02c741d067a883cb486d5caa01935bf6
nano_models.forEach(function(model) {
  test('[Nano ' + model.letter + '] Unstake: On-chain tx', zemu(model, async (sim, eth) => {

  const serializedTx = txFromEtherscan("0xf8ed82038285012a05f2008303098194c2e9d07f66a89c44062459a47a0d2dc038e4fb1680b8849bd9bbc6000000000000000000000000c228cefdf841defdbd5b3a18dfd414cc0dbfa0d80000000000000000000000000000000000000000000000000006f86db90ca000000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000008193a0b389f4fd684a2e9139c548c87c2fe4766195709c82ec35a0d3d40392adf8b564a048835090305cc4d69e41dd141553e729e7c76ec24f6089dd3d8282e2fd28228d");

  const tx = eth.signTransaction(
    "44'/60'/0'/0",
    serializedTx,
  );

  const right_clicks = 4;

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);
  // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots('.', model.name + '_unstake_on_chain_tx', [right_clicks, 0]);

  await tx;
  }));
});

