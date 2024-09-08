const vdf = require("../lib/index");

// Testing the event loop

(async () => {
  const intervalId = setInterval(() => {
    console.log("Event loop going");
  }, 1000);

  try {
    const result = await vdf.generateVDF(stringToHex("test"), 1000, 2048);
    console.log("VDF Result:", result);

    const check = await vdf.verifyVDF(stringToHex("test"), result, 1000, 2048);

    console.log(check);
  } catch (error) {
    console.error("Failed to generate VDF:", error);
  } finally {
    clearInterval(intervalId);
  }
})();

console.log("This runs before everything else");

function stringToHex(str) {
  const buf = Buffer.from(str, "utf8");
  return buf.toString("hex");
}
