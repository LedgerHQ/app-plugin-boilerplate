module.exports = {
  env: {
    es2021: true,
    node: true,
    jest: true,
  },
  extends: ["eslint:recommended", "prettier"],
  parserOptions: {
    sourceType: "module",
  },
  plugins: ["prettier"],
  rules: {
    "linebreak-style": ["error", "unix"],
    semi: ["error", "always"],
    "prettier/prettier": "error",
  },
};
